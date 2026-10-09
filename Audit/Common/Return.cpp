
#include "stdafx.h"
#include "cmn_resource.h"

#include "Return.h"
#include "Register.h"

#include "Profiler.h"

#include "portable.h"

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// static
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

eSeverity CReturn::m_severity = WARN_USER;
int CReturn::m_debug = 0;

bool CReturn::m_silent_mode = false;
CString CReturn::m_log_file = "";

CString CReturn::m_error_msg = "";
CString CReturn::m_hack_status_msg = "";

static CProfiler*	global_profiler = nullptr;


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

CReturn::CReturn()
	{ m_status = STATUS_OKAY; }

CReturn::CReturn( const CReturn& in_return )
	{ m_status = in_return.getStatus(); }

CReturn::CReturn( eReturnStatus in_status )
	{ m_status = in_status; }


CReturn::~CReturn()
	{ /* nothing to do */ }


const CReturn& CReturn::operator = ( const CReturn& in_ret )
{
	m_status = in_ret.getStatus();
	return *this;
}

const CReturn& CReturn::operator += ( const CReturn& in_ret )
{
	if ((int)in_ret.getStatus() > (int)m_status)
		m_status = in_ret.getStatus();
	return *this;
}

const CReturn& CReturn::operator = ( eReturnStatus status )
{
	m_status = status;
	return (*this);
}

const CReturn& CReturn::operator += ( eReturnStatus status )
{
	if (status > m_status)
		m_status = status;
	return (*this);
}


CReturn CReturn::SystemError( void )
{
	SystemError( GetLastError() );

	return *this;
}

CReturn CReturn::SystemError( int err )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString	prefix;
	CString	string;

	LPVOID lpMsgBuf = nullptr;

	prefix.LoadString( IDS_ERROR );
	m_status = STATUS_ERROR;

	if (err)
	{
		FormatMessage(     
							FORMAT_MESSAGE_ALLOCATE_BUFFER 
							| FORMAT_MESSAGE_FROM_SYSTEM 
							| FORMAT_MESSAGE_IGNORE_INSERTS,    
							nullptr,
							err,
							MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
							(LPTSTR) &lpMsgBuf,    
							0,    
							nullptr
						);
		string.Format( "%s: %s", prefix, (char*) lpMsgBuf );
	}
	else
	{
		string.Format( "%s: SYSTEM ERROR", prefix );
	}

	Record( WARN_INTERNAL, string );

	if (lpMsgBuf)
		LocalFree( lpMsgBuf );

	return *this;
}

CString CReturn::String( int string_id ) const
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString string;
	string.LoadString( string_id );

	return string;
}

// Where 'refix_id' and 'msg_id' are string ids in the resource file.
CString CReturn::Format( int prefix_id, int msg_id, ... ) const
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	va_list params;
	va_start( params, msg_id );

	CString	msg = Format( prefix_id, msg_id, params );

	return msg;
}

CString CReturn::Format( int prefix_id, int msg_id, va_list& params ) const
{
	CString	msg;

	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	{
		CString	vector;
		CString	prefix;

		vector.LoadString( msg_id );
		prefix.LoadString( prefix_id );

		char buffer[1024];
		vsprintf_s( buffer, 1024, vector, params );

		msg.Format( "%s: %s", prefix, buffer );
	}

	return msg;
}

CReturn CReturn::Fatal( int msg_id, ... )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	va_list params;
	va_start( params, msg_id );

	CString	msg = Format( IDS_FATAL, msg_id, params );

	EWMFatal( (LPCSTR) msg );
	LastErrorMsgSet( (LPCSTR) msg );

	m_status = STATUS_ERROR;

	return *this;
}

CReturn CReturn::User( int msg_id, ... )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	va_list params;
	va_start( params, msg_id );

	CString	msg = Format( IDS_ERROR, msg_id, params );

	m_status = STATUS_ERROR;

	EWMUser( (LPCSTR) msg );
	LastErrorMsgSet( (LPCSTR) msg );

	return *this;
}

CReturn CReturn::Internal( int msg_id, ... )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	va_list params;
	va_start( params, msg_id );

	CString	msg = Format( IDS_ERROR, msg_id, params );

	m_status = STATUS_ERROR;

	EWMWarning( (LPCSTR) msg );
	LastErrorMsgSet( (LPCSTR) msg );

	return *this;
}

CReturn CReturn::Diagnostic( const char* msg )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	EWMDiagnostic( msg );
	return *this;
}

CReturn CReturn::FDiagnostic( const char* format, ... )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	char buffer[1024];

	va_list params;
	va_start( params, format );

	vsprintf_s( buffer, 1024, format, params );

	EWMDiagnostic( buffer );
	return *this;
}

// 2006.12.17 (PE) -- Introduced to echo portal commands
//  regardless of other settings.
CReturn CReturn::Portal( const char* msg )
{
	EWMPortal( msg );
	return *this;
}

CReturn CReturn::Exception( const char* msg )
{
	CString final_msg;

	final_msg.Format( "EXCEPTION: %s", msg );

	EWMFatal( (LPCSTR) final_msg );
	LastErrorMsgSet( (LPCSTR) msg );

	m_status = STATUS_ERROR;

	return *this;
}

CReturn CReturn::UserWarn( int in_string_id, ... )
{
	char	buffer[1024];
	CString	vector;
	CString	prefix;
	CString	string;
	va_list	param;

	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	vector.LoadString( in_string_id );
	prefix.LoadString( IDS_WARNING );
	m_status = STATUS_WARNING;

	va_start( param, in_string_id );
	vsprintf_s( buffer, 1024, vector, param );

	string.Format( "%s: %s", prefix, buffer );

	EWMWarning( (LPCSTR) string );
	LastErrorMsgSet( (LPCSTR) string );

	return *this;
}

// Maybe this doesn't belong here... but it's sure handy
void CReturn::Block( int in_string_id, ... )
{
	char	buffer[1024];
	CString	vector;
	CString	string;
	va_list	param;

	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	vector.LoadString( in_string_id );

	va_start( param, in_string_id );
	vsprintf_s( buffer, 1024, vector, param );

	string.Format( "%s", buffer );

	MessageBox( nullptr, buffer, "Note", (MB_OK | MB_ICONINFORMATION));
}

// ==================================================================
//
void CReturn::Record( eSeverity severity, const CString& msg )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// 2009.08.08 (PE) -- 'm_error_msg' was introduced so that it
	// can be returned by CReturn::LastErrorMsg() for the purpose
	// of recording the reason(s) for failure of a part to nest.
	m_error_msg = msg;

	if ( m_silent_mode )
	{
		ErrorLog( severity, msg );
	}
	else if (CRegister::Debug("ewm") != 0)
	{
		bool show = (severity <= m_severity);
#if 0
		if (CRegister::Debug("Record") && severity != WARN_DIAGNOSTIC)
		{
			// Helps with debugging background processes.
			MessageBox( nullptr, msg, "CReturn::Record()", MB_OK );
		}
		show = true;
#endif
		if (show && (severity == WARN_DIAGNOSTIC))
		{
			if (msg.GetAt(0) == '*')
				show = (CRegister::Debug("Verbose") != 0);

		}

		if ( show )
			EWMMessageSend( (LPCSTR) msg );
	}
}

void CReturn::LastErrorMsgSet( const char* msg )
	{ CReturn::m_error_msg = ((msg == nullptr) ? "" : msg); }

const CString& CReturn::LastErrorMsg() const
	{ return CReturn::m_error_msg; }


void CReturn::LastStatusMsgSet( const char* msg )
	{ CReturn::m_hack_status_msg = ((msg == nullptr) ? "" : msg); }

const CString& CReturn::LastStatusMsg() const
	{ return CReturn::m_hack_status_msg; }


// ==================================================================

void CReturn::ErrorLevel( eSeverity level )
{
	if (level < WARN_USER)
		level = WARN_USER;

	m_severity = level;
}

eSeverity CReturn::ErrorLevel()
	{ return m_severity; }

void CReturn::Debug( int debug )
	{ m_debug = debug; }

int CReturn::Debug( void )
	{ return m_debug; }

void CReturn::SilentMode( bool enable )
	{ m_silent_mode = enable; }

bool CReturn::IsSilentMode()
	{ return m_silent_mode; }

void CReturn::LogFile( const CString& file )
	{ m_log_file = file; }

void CReturn::ErrorLog( eSeverity severity, const CString& msg )
{
	if ( !m_log_file.IsEmpty() )
	{
		bool show = (severity <= m_severity);
		if (show && (severity == WARN_DIAGNOSTIC))
		{
			if (msg.GetAt(0) == '*')
				show = (CRegister::Debug("Verbose") != 0);

		}

		if ( show )
		{
			FILE* f = fopen( m_log_file, "a+" );
			if (f != nullptr)
			{
				fprintf( f, "%s\n", msg );
				fclose( f );
			}
		}
	}
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

static char g_file_info[1024];

const char* FileInfo( const char* file, int line, const char* func )
{
	sprintf_s( g_file_info, 1023, "%s(%d): %s()", file, line, func );
	return g_file_info;
}
