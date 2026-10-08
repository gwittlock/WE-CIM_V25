#ifndef _BINARY_FILE_H
#define _BINARY_FILE_H

// ==================================================================
//		Binary File
//
//		Generic Binary-oriented File Access Object
//
// ==================================================================

#include "stdafx.h"
#include <afx.h>

#include "text_file.h"

#include "Return.h"

// ==================================================================

class dllExport CBinaryFile
{
public:
	CBinaryFile( void );
	~CBinaryFile( void );

	CReturn	Open( const CString& in_name, eFileMode in_mode );
	CReturn Close( void );

	CReturn	Read( BYTE* buf, int num );
	CReturn	Write( BYTE* buf, int num );

	BOOL	isEOF( void ) const				{ return m_eof; }

	DWORD	getFlag( void ) const			{ return m_flag; }
	void	setFlag( DWORD in_flag )		{ m_flag |= in_flag; }
	void	clrFlag( DWORD in_flag )		{ m_flag &= ~in_flag; }
	BOOL	tstFlag( DWORD in_flag ) const	{ return m_flag & in_flag; }

	CString	getName( void ) const			{ return m_filename; }

protected:
	void		reset_buffers( void );
	CReturn		fill_cache( void );

	CString		m_filename;

	eFileMode	m_mode;
	CFile		m_file;		// File in process
	FILE*		m_stream;	// Stream file... test
	DWORD		m_flag;		// Various control flags, tbd

	int			m_bufidx;	// Index within buffer; 0<=idx<len
	int			m_buflen;	// Length of buffer
	int			m_filelen;	// Remaining length in file
	BYTE*		m_buf;		// File cache... my version

	BOOL		m_eof;		// TRUE if out of file...
};

#endif