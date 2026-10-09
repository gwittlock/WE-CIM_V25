#ifndef _TEXT_FILE_H
#define _TEXT_FILE_H

// ==================================================================
//		Text File
//
//		Generic Text-oriented File Access Object
//
// ==================================================================

#include "stdafx.h"
#include <afx.h>

#include "Return.h"

// ==================================================================

enum eFileMode
{
	FILEMODE_ERROR,		// Problem
	FILEMODE_READ,		// Read existing
	FILEMODE_WRITE,		// Write new
	FILEMODE_APPEND		// Append to existing
};



// ==================================================================

class dllExport CTextFile
{
public:
	CTextFile( void );
	~CTextFile( void );

	CReturn	Open( const CString& in_name, eFileMode in_mode );
	CReturn Close( void );

	CString	ReadLine( void );
	CReturn	WriteLine( CString in_line );
	CReturn	Write( CString in_text );

	int		getLineNum( void ) const		{ return m_lineno; }
	bool	isEOF( void ) const				{ return m_eof; }

	DWORD	getFlag( void ) const			{ return m_flag; }
	void	setFlag( DWORD in_flag )		{ m_flag |= in_flag; }
	void	clrFlag( DWORD in_flag )		{ m_flag &= ~in_flag; }
	bool	tstFlag( DWORD in_flag ) const	{ return ((m_flag & in_flag) != 0); }

	CString	getName( void ) const			{ return m_filename; }

protected:
	void		reset_buffers( void );
	CReturn		fill_cache( void );

	CString		m_filename;

	eFileMode	m_mode;
	CFile		m_file;		// File in process
	DWORD		m_flag;		// Various control flags, tbd

	int			m_lineno;	// Physical line number

	int			m_bufidx;	// Index within buffer; 0<=idx<len
	int			m_buflen;	// Length of buffer
	int			m_filelen;	// Remaining length in file
	BYTE*		m_buf;		// File cache... my version

	bool		m_eof;		// TRUE if out of file...
};

#endif