#if !defined(_RETURN_H)
#define _RETURN_H

// ==================================================================
//		Return
//
//	Temporary, initial, test version of CReturn status class
//
// ==================================================================

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// ==================================================================

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
	void SystemError( void );
	void SystemError( int err );

	void Fatal( int in_string_id, ... );
	void User( int in_string_id, ... );
	void Internal( int in_string_id, ... );
	void Diagnostic( const char* msg );

	// This method will set this objects' status to STATUS_WARNING.
	void UserWarn( int in_string_id, ... );

	eReturnStatus getStatus( void ) const					{ return m_status; }
	BOOL	isOkay( void ) const							{ return m_status == STATUS_OKAY; }
	BOOL	IsOk( void ) const								{ return m_status == STATUS_OKAY; }
	BOOL	isError( void ) const							{ return m_status == STATUS_ERROR; }
	BOOL	isDone( void ) const							{ return (m_status == STATUS_DONE) || (m_status == STATUS_ERROR); }

	// NOTE:  May be not okay, but not error -- warning mode!
	virtual ~CReturn();

public:

	static void ErrorLevel( eSeverity level );
	static eSeverity ErrorLevel();

	static void Debug( BOOL on );

	// Profiler tools
	static void pDump( void );
	static int Profile( const CString&	name );
	static void In( int idx );
	static void Out( int idx );

private:
	// Disabled.
	int operator == ( const CReturn& ) const;
	int operator != ( const CReturn& ) const;

private:
	
	void Record( eSeverity severity, const CString& msg );
	LPWORD	align_dword( LPWORD in_ptr );
	int		set_wide_string( LPWORD in_dest, LPSTR in_src );

	eReturnStatus	m_status;

	static CDialog* m_dlg;
	static eSeverity m_severity;
};

#endif

