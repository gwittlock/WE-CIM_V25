// ============================================================================
//		Serial
//
//	Generic interface to the serial port, include low-level bit-twiddling
//
//	Copyright 2001, Simulated Reality Systems, LLC
//
// ============================================================================

#include "Stdafx.h"
#include "Serial.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// ============================================================================
// Construction/Destruction
//
CSerial::CSerial()
{
	m_comm = INVALID_HANDLE_VALUE;

	m_read_buflen = 0;
	m_read_buf = NULL;
	m_read_head = -1;
	m_read_tail = -1;

	m_timeout = 0;
}

CSerial::~CSerial()
{
	Close();

	if (m_read_buf) { delete m_read_buf; m_read_buf = NULL; }
}

// ============================================================================
BOOL
CSerial::Init( 
	int	in_read_buflen )
{
	m_read_buflen = in_read_buflen;
	m_read_buf = new BYTE[m_read_buflen];
	if (!m_read_buf)
		return FALSE;

	m_read_head = 0;
	m_read_tail = 0;

	return TRUE;
}

// ============================================================================
BOOL	
CSerial::ConfigDialog(
	const CString&	in_portname )
{
	m_portname = in_portname;

	DWORD size = sizeof( m_config );
	GetDefaultCommConfig( m_portname, &m_config, &size );

	if (CommConfigDialog( m_portname, NULL, &m_config ))
	{
		SetDefaultCommConfig( m_portname, &m_config, sizeof(m_config) );
		return TRUE;
	}
	return FALSE;
}

// ============================================================================
BOOL
CSerial::Open( void )
{
	m_comm = CreateFile(	m_portname,
								GENERIC_READ
								| GENERIC_WRITE,
								0,					// Exclusive access
								NULL,				// No security
								OPEN_EXISTING,
								0,					// No attributes
								NULL );				// Not copied from another
	if (m_comm == INVALID_HANDLE_VALUE)
	{
		error_message( GetLastError() );
		return FALSE;
	}

	DWORD size = 0;
	GetDefaultCommConfig( m_portname, &m_config, &size );
	m_config.dcb.XoffChar = 0x13;
	m_config.dcb.XonChar = 0x11;
	SetCommConfig( m_comm, &m_config, sizeof(m_config) );

	COMMTIMEOUTS	timeout;
	GetCommTimeouts( m_comm, &timeout );
	timeout.ReadIntervalTimeout			= 0;
	timeout.ReadTotalTimeoutMultiplier	= 0;
	timeout.ReadTotalTimeoutConstant	= READ_TIMEOUT;
	timeout.WriteTotalTimeoutMultiplier	= 0;
//	timeout.WriteTotalTimeoutConstant	= WRITE_TIMEOUT;
	timeout.WriteTotalTimeoutConstant	= INFINITE;
	SetCommTimeouts( m_comm, &timeout );

	DCB state;
	GetCommState( m_comm, &state );
	state.XoffChar = 0x13;
	state.XonChar = 0x11;
	SetCommState( m_comm, &state );

	m_read_head = 0;
	m_read_tail = 0;

	return TRUE;
}

// ============================================================================
BOOL
CSerial::Close( void )
{
	if (m_comm != INVALID_HANDLE_VALUE)
		CloseHandle( m_comm );
	m_comm = INVALID_HANDLE_VALUE;

	return TRUE;
}

// ============================================================================
void
CSerial::Pump( void )
{
	BYTE	val;
	DWORD	read;

	if (m_comm == INVALID_HANDLE_VALUE)
		return;

	while (TRUE)
	{
		if (!ReadFile( m_comm, &val, 1, &read, NULL ))
			return;
		if (read != 1)
			return;

		m_read_buf[m_read_tail++] = val;
		if (m_read_tail >= m_read_buflen)
			m_read_tail = 0;
	}
}

// ============================================================================
BOOL			
CSerial::Send( 
	BYTE*	in_block, 
	int	in_len )
{
	DWORD		written;

	if (!WriteFile( m_comm, in_block, in_len, &written, NULL ))
	{
		error_message( GetLastError() );
		return FALSE;
	}

	if (written != (DWORD)in_len)
	{
		error_message( GetLastError() );
		return FALSE;
	}

	return TRUE;
}

// ============================================================================
BOOL
CSerial::Receive( 
	BYTE*	out_block, 
	int	in_len )
{
	int	got_len;
	BYTE*	ptr;
	int	retry;

	retry = (m_timeout / (READ_TIMEOUT + RETRY_DELAY)) + 1;

	while (retry--)
	{
		Pump(); 

		if (m_read_tail < m_read_head)
			got_len = (m_read_buflen - m_read_head) + m_read_tail;
		else
			got_len = m_read_tail - m_read_head;

		if (got_len >= in_len)
		{
			ptr = out_block;
			while (in_len--)
			{
				*ptr++ = m_read_buf[m_read_head++];
				if (m_read_head >= m_read_buflen)
					m_read_head = 0;
			}
			return TRUE;
		}
	}

	return FALSE;
}

// ============================================================================
//		error_message
//
//	TODO:  Make a global error management class?
//
void
CSerial::error_message(
	int	in_error )
{
	LPVOID	err_msg;

	FormatMessage(	FORMAT_MESSAGE_ALLOCATE_BUFFER
						| FORMAT_MESSAGE_FROM_SYSTEM,
						NULL,
						in_error, 
						MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
						(LPTSTR)&err_msg,
						0,
						NULL );
	MessageBox( NULL, (LPCSTR)err_msg, NULL, MB_OK|MB_ICONINFORMATION );
	LocalFree( err_msg );
}


// ============================================================================

BOOL		
CSerial::DSRbit( void ) const
{
	DWORD bit = 0;

	if (m_comm != INVALID_HANDLE_VALUE)
		GetCommModemStatus( m_comm, &bit );

	return (bit & MS_DSR_ON) > 0;
}

void		
CSerial::TXbit( BOOL bit ) const
{
	if (m_comm == INVALID_HANDLE_VALUE)
		return;

	if (bit)
		EscapeCommFunction( m_comm, SETBREAK );
	else
		EscapeCommFunction( m_comm, CLRBREAK );
}

void
CSerial::DTRbit( BOOL bit ) const
{
	if (m_comm == INVALID_HANDLE_VALUE)
		return;

	if (bit)
		EscapeCommFunction( m_comm, SETDTR );
	else
		EscapeCommFunction( m_comm, CLRDTR );
}

void	
CSerial::RTSbit( BOOL bit ) const
{
	if (m_comm == INVALID_HANDLE_VALUE)
		return;

	if (bit)
		EscapeCommFunction( m_comm, SETRTS );
	else
		EscapeCommFunction( m_comm, CLRRTS );
}

