#if !defined(_CMDPROCESS_H)
#define _CMDPROCESS_H

// ==================================================================
//		CmdProcess
//
//	Command-driven process.  All generic, routable procedures 
//	must be derived from Command Process.  The Execute() method
//	then does whatever work is desired.
//
//	The owner of a CmdProcess must register the process
//	object with a routing list.
//
// ==================================================================

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// ==================================================================

class CCmdProcess
{
public:
	CCmdProcess();
	virtual ~CCmdProcess();

	virtual CReturn	Execute( CCommand& in_cmd );

protected:

private:
	// Disabled.
	CCmdProcess( const CCmdProcess& );
	const CCmdProcess& operator = ( const CCmdProcess& );
	int operator == ( const CCmdProcess& ) const;
	int operator != ( const CCmdProcess& ) const;

private:
};

#endif

