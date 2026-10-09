// ==================================================================
//		RouteList
//
//	List of commands (routing names) with associated recipient
//	object.  Used to register commands, and then route command
//	objects to them.
//
// ==================================================================

#include "stdafx.h"
#include "cmn_resource.h"

#define _IMPORT_ 1
#include "RouteList.h"

// ==================================================================
//
//	Pointer to a generic command-driven process.
//	TODO:  Add additional parameters?
//
// ==================================================================

class CRouteEntry
{
public:

	CRouteEntry()
	{
		m_process = NULL;
		m_subrouter = NULL;
		m_hit = 0;
	}

	virtual ~CRouteEntry()
	{
		// Introduced this breakpoint here because we
		// were crashing with "scalar deleting destructor"
		// Unable to resolve, however.
		// See also CRouteList::Reset() :-(
		int foo = 0;
	}

	void Name( const CString& name )
		{ m_name = name; }

	const CString& Name( void ) const
		{ return m_name; }

	void Process( pProcess process )
		{ m_process = process; }

	pProcess Process( void ) const
		{ return m_process; }

	void Subrouter( CRouteList* sub )
		{ m_subrouter = sub; }

	CRouteList*	Subrouter( void ) const
		{ return m_subrouter; }

	void Use( void )
		{ m_hit++; }

	int Hits( void ) const
		{ return m_hit; }

private:

	CString		m_name;
	pProcess	m_process;
	CRouteList*	m_subrouter;

	int			m_hit;
};



// ==================================================================

CRouteList::CRouteList()
{
}

CRouteList::~CRouteList()
{
	Reset();
}

// ==================================================================
void
CRouteList::Reset( void )
{
	m_routelist.DestructiveFlush();
}

// ==================================================================
//		addProcess
//
//	Add a named command processor to the list.
//	The name is duplicated in the list.  The process address is
//	only a reference here; it is not owned by the route list, and
//	will not be deleted by it.
//
CReturn	
CRouteList::addProcess( 
	const CString&	in_name, 
	pProcess		in_process )
{
	CRouteEntry* new_entry;
	CRouteEntry* entry;
	int	idx;

	if (!in_process)
		return CReturn( STATUS_ERROR );

	new_entry = new CRouteEntry;
	new_entry->Name( in_name );
	new_entry->Process( in_process );

	int num = m_routelist.Count();
	for (idx=0; idx<num; idx++)
	{
		entry = m_routelist[idx];
		if (entry->Name().CompareNoCase( in_name ) > 0)
		{
			m_routelist.InsertBefore( idx, new_entry );
//			m_route_list.InsertAt( idx, in_name );
//			m_process_list.InsertAt( idx, in_process );
//			m_subrouter_list.InsertAt( idx, (CRouteList*)NULL );

			return CReturn( STATUS_OKAY );
		}
	}

	m_routelist.Append( new_entry );
//	m_route_list.Add( in_name );
//	m_process_list.Add( in_process );
//	m_subrouter_list.Add( NULL );

	return CReturn( STATUS_OKAY );
}

// ==================================================================
//		addSubrouter
//
//	Add a named command sub-router...
//	The name is duplicated in the list.  The process address is
//	only a reference here; it is not owned by the route list, and
//	will not be deleted by it.
//
CReturn	
CRouteList::addSubrouter( 
	const CString&	in_name, 
	CRouteList*		in_subrouter )
{
	CRouteEntry* new_entry;
	CRouteEntry* entry;
	int	idx;

	if (!in_subrouter)
		return CReturn( STATUS_ERROR );

	new_entry = new CRouteEntry;
	new_entry->Name( in_name );
	new_entry->Subrouter( in_subrouter );

	int num = m_routelist.Count();
	for (idx=0; idx<num; idx++)
	{
		entry = m_routelist[idx];
		if (entry->Name().CompareNoCase( in_name ) > 0)
		{
			m_routelist.InsertBefore( idx, new_entry );
//			m_route_list.InsertAt( idx, in_name );
//			m_process_list.InsertAt( idx, (pProcess)NULL );
//			m_subrouter_list.InsertAt( idx, in_subrouter );

			return CReturn( STATUS_OKAY );
		}
	}

	m_routelist.Append( new_entry );
//	m_route_list.Add( in_name );
//	m_process_list.Add( NULL );
//	m_subrouter_list.Add( in_subrouter );

	return CReturn( STATUS_OKAY );
}



// ==================================================================
//		Dispatch
//
//	Given a command, match the next routing location against the
//	list.  If the processes is found, call its Execute() method with
//	the command.  If the process is NOT found, backup the command,
//	and return an error.
//
//	NOTE:  This is the same searching mechanism used in CVarList
//	TODO:  Template for binary search lists?
//
CReturn	
CRouteList::Dispatch( 
	CCommand* io_cmd )
{
	int		num;
	int		width;
	int		idx;
	CReturn	ret;
	CString	route_name;

	ret = io_cmd->nextRoute(&route_name);
	if (ret.isError())
		return ret;

	// ----------------------------------------------------
	// Prepare the variables for a binary search; forces
	//	width to be a power of 2, which makes for an easier
	//	search algorithm.
	// TODO:  Remove this prep section for efficiency, and 
	//	polish the algorithm to cope with odd widths.
	//
	num = m_routelist.Count();
	width = 1;
	while (num)
	{
		width <<= 1;
		num >>= 1;
	}

	num = m_routelist.Count();
	width >>= 1;
	idx = width-1;

	// ----------------------------------------------------
	// Now perform the search...
	//
	while (width)
	{
		width >>= 1;

		if (idx >= num)
		{
			// Array overrun is an automatic back-up
			idx -= width;
		}
		else
		{
			CRouteEntry* entry = m_routelist[idx];
			switch (ISGN(entry->Name().CompareNoCase( route_name )))
			{
				case 0:
				{
					if (entry->Subrouter())
						return entry->Subrouter()->Dispatch( io_cmd );

					entry->Use();

					return (entry->Process())( io_cmd );
				}

				case -1:
					idx += width;
					break;
				case 1:
					idx -= width;
					break;
			}
		}
	}
	//
	// Error case, backup and die
	//
	io_cmd->Backup();
	ret.Internal( IDS_CMD_ROUTE_ERROR, route_name );

	return ret;
}

// ==================================================================
//		Verify
//
//	Given a command, match the next routing location against the
//	list.  If the processes is found, returns TRUE.  Only verifies
//	first-level route in the command; it is assumed that if the 
//	first dispatch level exists, the sub commands will also work.
//
bool CRouteList::Verify( CCommand* io_cmd )
{
	int		num;
	int		width;
	int		idx;
	CReturn	ret;
	CString	route_name;

	ret = io_cmd->nextRoute(&route_name);
	if (ret.isError())
		return FALSE;

	io_cmd->Backup();

	// ----------------------------------------------------
	// Prepare the variables for a binary search; forces
	//	width to be a power of 2, which makes for an easier
	//	search algorithm.
	// TODO:  Remove this prep section for efficiency, and 
	//	polish the algorithm to cope with odd widths.
	//
	num = m_routelist.Count();
	width = 1;
	while (num)
	{
		width <<= 1;
		num >>= 1;
	}

	num = m_routelist.Count();
	width >>= 1;
	idx = width-1;

	// ----------------------------------------------------
	// Now perform the search...
	//
	while (width)
	{
		width >>= 1;

		if (idx >= num)
		{
			// Array overrun is an automatic back-up
			idx -= width;
		}
		else
		{
			CRouteEntry* entry = m_routelist[idx];
			switch (ISGN(entry->Name().CompareNoCase( route_name )))
			{
				case 0:
					return TRUE;

				case -1:
					idx += width;
					break;
				case 1:
					idx -= width;
					break;
			}
		}
	}

	return FALSE;
}



// ==================================================================

void	
CRouteList::Dump( 
	const CString& filepath )
{
	CReturn ret;
	CTextFile	file;

	ret = file.Open( filepath, FILEMODE_WRITE );
	if (!ret.isOkay())
		return;

	Dump( file, CString("") );

	file.Close();
}

void
CRouteList::Dump(
	CTextFile&	file,
	CString&	prefix )
{
	int num = m_routelist.Count();
	for (int idx=0; idx<num; idx++)
	{
		CRouteEntry* entry = m_routelist[idx];

		CRouteList* subroute = entry->Subrouter();
		if (subroute)
		{
			CString new_prefix = prefix + entry->Name() + ":";
			subroute->Dump( file, new_prefix );
		}
		else
		{
			CString count;
			if (entry->Hits())
				count.Format("%4d  ", entry->Hits() );
			else
				count = "      ";
			file.Write( count );

			file.Write( prefix );
			file.Write( entry->Name() );
			file.WriteLine( ":" );
		}
	}
}
