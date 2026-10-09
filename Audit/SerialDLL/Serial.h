#if !defined(_SERIAL_H)
#define _SERIAL_H
#pragma once

// ============================================================================
//		Serial
//
//	Generic interface to the serial port, include low-level bit-twiddling
//
//	Copyright 2001, Simulated Reality Systems, LLC
//
// ============================================================================

#define SERIAL_READ_BUFLEN		1024

#define READ_TIMEOUT	50
#define WRITE_TIMEOUT	50

#define RETRY_DELAY		50

// ============================================================================

class CSerial  
{
public:
	CSerial();
	virtual ~CSerial();

	BOOL	Init( int in_read_buflen = SERIAL_READ_BUFLEN );

	BOOL	ConfigDialog( const CString& in_portname );

	BOOL	Open( void );
	BOOL	Close( void );

	void	Pump( void );

	BOOL	Send( BYTE in_byte )						{ return Send( &in_byte, 1 ); }
	BOOL	Send( CString& in_text )					{ return Send( (BYTE*)(LPCSTR)in_text, in_text.GetLength() ); }
	BOOL	Send( BYTE* in_block, int in_len );

	BOOL	Receive( BYTE* out_byte )					{ return Receive( out_byte, 1 ); }
	BOOL	Receive( BYTE* out_block, int in_len );

	// -------------------------
	// Simple access
	//
	void			PortName( const CString& in_portname )	{ m_portname = in_portname; }
	const CString&	PortName( void ) const					{ return m_portname; }

	void	Timeout( int in_timeout )						{ m_timeout = in_timeout; }
	int		Timeout( void ) const							{ return m_timeout; }

	// -------------------------
	// Low-level bit twiddling
	//
	BOOL		DSRbit( void ) const;

	void		TXbit( BOOL bit ) const;
	void		DTRbit( BOOL bit ) const;
	void		RTSbit( BOOL bit ) const;

private:
	void error_message( int in_error );

	HANDLE		m_comm;
	CString		m_portname;
	COMMCONFIG	m_config;

	int			m_read_buflen;
	BYTE*		m_read_buf;
	int			m_read_head;
	int			m_read_tail;

	int			m_timeout;
};




#endif