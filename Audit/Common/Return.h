#pragma once

// EWM messaging used to be handled by CReturn so
// "ewmmsg.h" is include here for convenience because
// "return.h" is likely included almost everywhere.
#include "ewmmsg.h"

enum eReturnStatus
{
	STATUS_NONE,
	STATUS_OKAY,
	STATUS_DONE,
	STATUS_WARNING,
	STATUS_ERROR
};

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// The portal provides a command that controls the output of errors
// to the output window based upon the severity level of the message.
//
// Fatal error messages must absolutely be seen.  Failure to
// initialize the Java Virtual Machine is one such example.
//
// User error messages provide the user with information that
// helps him to correct/work-around some circumstance.  Java compiler
// messages are one such example.  Attempts to use an open chain in a
// pocketing operation is another example.
//
// Internal error messages should not normally be seen by the user.
// These messages are used primarily by developers for debugging purposes.
// There are occassions where a client service representative may need to
// see these messages.  This can be achieved by executing the portal 
// command 'Admin:Error: level=%d'
//
// Diagnostic messages are like internal error messages but simply
// provide more detail than someone would normally want to see.
//
// These levels of error messages have the associated class methods
//    Fatal(), User(), Internal() and Diagnostic()
//
// The levels of error messages that are displayed are controlled by
//    ErrorLevel()
//
enum eSeverity
{
	WARN_FATAL,
	WARN_USER,
	WARN_INTERNAL,
	WARN_DIAGNOSTIC
};

// TODO:  Determine "real" ID's that won't cause problems
//const int RETURN_LIST_ID = 1025;

// ==================================================================

class CReturnDlg;

class dllExport CReturn
{
public:

	CReturn();
	CReturn( const CReturn& );
	CReturn( eReturnStatus in_status );

	const CReturn& operator = ( const CReturn& );
	const CReturn& operator += ( const CReturn& );

	const CReturn& operator = ( eReturnStatus );
	const CReturn& operator += ( eReturnStatus );

	void setStatus( eReturnStatus in_status )		{ m_status = in_status; }

	// These methods will set this objects' status to STATUS_ERROR.
	CReturn SystemError( void );
	CReturn SystemError( int err );

	CString String( int string_id ) const;
	CString Format( int prefix_id, int msg_id, ... ) const;

	CReturn Fatal( int in_string_id, ... );
	CReturn User( int in_string_id, ... );
	CReturn Internal( int in_string_id, ... );
	CReturn Diagnostic( const char* msg );
	CReturn FDiagnostic( const char* msg, ... );
	CReturn Portal( const char* msg );
	CReturn Exception( const char* msg );

	// This method will set this objects' status to STATUS_WARNING.
	CReturn UserWarn( int in_string_id, ... );

	void Block( int in_string_id, ... );

	eReturnStatus getStatus( void ) const	{ return m_status; }
	bool	isOkay( void ) const			{ return m_status == STATUS_OKAY; }
	bool	IsOk( void ) const				{ return m_status == STATUS_OKAY; }
	bool	isError( void ) const			{ return m_status == STATUS_ERROR; }
	bool	isDone( void ) const			{ return (m_status == STATUS_DONE) || (m_status == STATUS_ERROR); }

	void LastErrorMsgSet( const char* msg );
	const CString& LastErrorMsg() const;
	
	void LastStatusMsgSet( const char* msg );
	const CString& LastStatusMsg() const;

	// NOTE:  May be not okay, but not error -- warning mode!
	virtual ~CReturn();

public:

	static void ErrorLevel( eSeverity level );
	static eSeverity ErrorLevel();

	static void Debug( int debug );
	static int Debug( void );

	static void SilentMode( bool enable );
	static bool IsSilentMode();

	static void LogFile( const CString& file );
	static void ErrorLog( eSeverity severity, const CString& msg );

private:

	CString Format( int prefix_id, int msg_id, va_list& params ) const;

	void Record( eSeverity severity, const CString& msg );

private:  // disabled.

	int operator == ( const CReturn& ) const;
	int operator != ( const CReturn& ) const;

private:

	static eSeverity	m_severity;
	static int			m_debug;

	static bool			m_silent_mode;
	static CString		m_log_file;

	static CString		m_error_msg;
	static CString		m_hack_status_msg;

private:

	eReturnStatus	m_status;
};

/*
MSVC++ 11.0 _MSC_VER = 1700 (Visual Studio 2012)
MSVC++ 10.0 _MSC_VER = 1600 (Visual Studio 2010)
MSVC++ 9.0  _MSC_VER = 1500 (Visual Studio 2008)
MSVC++ 8.0  _MSC_VER = 1400 (Visual Studio 2005)
MSVC++ 7.1  _MSC_VER = 1310 (Visual Studio 2003)
MSVC++ 7.0  _MSC_VER = 1300
MSVC++ 6.0  _MSC_VER = 1200
MSVC++ 5.0  _MSC_VER = 1100
*/

dllExport const char* FileInfo( const char* file, int line, const char* func );

#if (_MSC_VER == 1200)
	#define FILE_INFO FileInfo( __FILE__, __LINE__, "" )
#else
	#define FILE_INFO FileInfo( __FILE__, __LINE__, __FUNCTION__ )
#endif
