// ==================================================================
//		CReturn
//
// ==================================================================

#include "stdafx.h"
#include "cmn_resource.h"

#include "Return.h"

#include "Profiler.h"

#if BEFORE
#else
#include "IpcSender.h"
static CIpcSender g_ipcSender;
#endif

// ==================================================================

CDialog* CReturn::m_dlg = NULL;
eSeverity CReturn::m_severity = WARN_USER;

static BOOL g_debug = FALSE;

static CProfiler*		global_profiler = NULL;

// ==================================================================


CReturn::CReturn()
	: m_status( STATUS_OKAY )
{
}

CReturn::CReturn(
	const CReturn& in_return )
{
	m_status = in_return.getStatus();
}

CReturn::CReturn(
	eReturnStatus in_status )
{
	m_status = in_status;
}


CReturn::~CReturn()
{
}


const CReturn&
CReturn::operator = (
	const CReturn& in_ret )
{
	m_status = in_ret.getStatus();
	return *this;
}

const CReturn&
CReturn::operator += (
	const CReturn& in_ret )
{
	if ((int)in_ret.getStatus() > (int)m_status)
		m_status = in_ret.getStatus();
	return *this;
}

const CReturn&
CReturn::operator = ( eReturnStatus status )
{
	m_status = status;
	return (*this);
}

const CReturn&
CReturn::operator += ( eReturnStatus status )
{
	if (status > m_status)
		m_status = status;
	return (*this);
}


void	
CReturn::SystemError( void )
{
	SystemError( GetLastError() );
}

void
CReturn::SystemError( int err )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString	prefix;
	CString	string;

	LPVOID lpMsgBuf = NULL;

	prefix.LoadString( IDS_ERROR );
	m_status = STATUS_ERROR;

	if (err)
	{
		FormatMessage(     
							FORMAT_MESSAGE_ALLOCATE_BUFFER 
							| FORMAT_MESSAGE_FROM_SYSTEM 
							| FORMAT_MESSAGE_IGNORE_INSERTS,    
							NULL,
							err,
							MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
							(LPTSTR) &lpMsgBuf,    
							0,    
							NULL
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
}

void	
CReturn::Fatal( int in_string_id, ... )
{
	char	buffer[1024];
	CString	vector;
	CString	prefix;
	CString	string;
	va_list	param;

	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	vector.LoadString( in_string_id );
	prefix.LoadString( IDS_FATAL );
	m_status = STATUS_ERROR;

	va_start( param, in_string_id );
	vsprintf( buffer, vector, param );

	string.Format( "%s: %s", prefix, buffer );

	Record( WARN_FATAL, buffer );
}

void	
CReturn::User( int in_string_id, ... )
{
	char	buffer[1024];
	CString	vector;
	CString	prefix;
	CString	string;
	va_list	param;

	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	vector.LoadString( in_string_id );
	prefix.LoadString( IDS_ERROR );
	m_status = STATUS_ERROR;

	va_start( param, in_string_id );
	vsprintf( buffer, vector, param );

	string.Format( "%s: %s", prefix, buffer );

	Record( WARN_USER, string );
}

void	
CReturn::Internal( int in_string_id, ... )
{
	char	buffer[1024];
	CString	vector;
	CString	prefix;
	CString	string;
	va_list	param;

	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	vector.LoadString( in_string_id );
	prefix.LoadString( IDS_ERROR );
	m_status = STATUS_ERROR;

	va_start( param, in_string_id );
	vsprintf( buffer, vector, param );

	string.Format( "%s: %s", prefix, buffer );

	Record( WARN_INTERNAL, string );
}

void	
CReturn::Diagnostic( const char* msg )
{
	Record( WARN_DIAGNOSTIC, msg );
}
void	
CReturn::UserWarn( int in_string_id, ... )
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
	vsprintf( buffer, vector, param );

	string.Format( "%s: %s", prefix, buffer );

	Record( WARN_USER, string );
}


//
// Maybe this doesn't belong here... but it's sure handy
//
int
CReturn::Question( int in_string_id, int mode, ... )
{
	char	buffer[1024];
	CString	vector;
	CString	prefix;
	CString	string;
	va_list	param;

	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	vector.LoadString( in_string_id );
	prefix.LoadString( IDS_NOTE );

	va_start( param, in_string_id );
	vsprintf( buffer, vector, param );

	string.Format( "%s: %s", prefix, buffer );

//	Record( WARN_USER, string );
	return MessageBoxA( NULL, buffer, "Panel Nest", mode );
}


// ==================================================================
//
void
CReturn::Record( eSeverity severity, const CString& msg )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	BOOL show = (severity <= m_severity);

#ifdef _DEBUG
	if (g_debug && severity != WARN_DIAGNOSTIC)
	{
		// Helps with debugging background processes.
		MessageBoxA( NULL, msg, "CReturn::Record()", MB_OK );
	}
	show = TRUE;
#endif

	if ( show )
	{
		g_ipcSender.SendMsg( MSGCMD_TEXT, msg );
	}
}

LPWORD
CReturn::align_dword( LPWORD in_ptr )
{
	ULONG		ul;

	ul = (ULONG)in_ptr;
	ul += 3;
	ul >>= 2;
	ul <<= 2;

	return (LPWORD)ul;
}

int
CReturn::set_wide_string(
	LPWORD	in_dest,
	LPSTR		in_src )
{
	int	length = lstrlen( in_src );

	return MultiByteToWideChar( GetACP(), MB_PRECOMPOSED, in_src, length, in_dest, length) + 1;
}


// ==================================================================
void
CReturn::ErrorLevel( eSeverity level )
{
	if (level < WARN_USER)
		level = WARN_USER;

	m_severity = level;
}

eSeverity
CReturn::ErrorLevel()
{
	return m_severity;
}

void
CReturn::Debug( BOOL on )
{
	g_debug = on;
}

void
CReturn::pDump( void )
{
	if (global_profiler)
		global_profiler->Dump();
}

int
CReturn::Profile( 
	const CString&	name )
{
	if (!global_profiler)
		global_profiler = new CProfiler;

	return global_profiler->Register( name );
}

void
CReturn::In( int idx )
{
	global_profiler->In( idx );
}

void
CReturn::Out( int idx )
{
	global_profiler->Out( idx );
}
