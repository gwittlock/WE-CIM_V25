#ifndef _BITSTREAMFILE_H
#define _BITSTREAMFILE_H

#include "stdafx.h"
#include <afx.h>

#include "text_file.h"

#include "Return.h"

// ==================================================================

class dllExport CBitstreamFile
{
public:

	CBitstreamFile( void );

	CReturn	Open( const CString& in_name, eFileMode in_mode );
	CReturn Close( void );

	// Get the length of this file.
	long	Length();

	// Do a file seek and load the buffer.
	CReturn Seek( LONG lOff, UINT from );

	// Seek relative to the beginning of the internal buffer.
	CReturn	BufferSeek( int bytes, int bits );

	int		FileIndex()		{ return m_fileidx; }
	int		ByteIndex()		{ return m_byteidx; }
	int		BitIndex()		{ return m_bitidx; }

	void	SyncByte();

	CReturn	ReadBytes( BYTE* buf, int num );
	CReturn	WriteBytes( BYTE* buf, int num );

	CReturn	ReadByte( BYTE*	buf );
	CReturn ReadWord( WORD* buf );

	CReturn ReadBits( BYTE* buf, int num );
	CReturn ReadBit( BYTE* buf );

	bool	isEOF( void ) const				{ return m_eof; }

	DWORD	getFlag( void ) const			{ return m_flag; }
	void	setFlag( DWORD in_flag )		{ m_flag |= in_flag; }
	void	clrFlag( DWORD in_flag )		{ m_flag &= ~in_flag; }
	bool	tstFlag( DWORD in_flag ) const	{ return ((m_flag & in_flag) != 0); }

	CString	getName( void ) const			{ return m_filename; }

	void	BitCounter( bool on );
	int		BitCountGet()					{ return m_bitsUsed; }

	void TestBufferSet( BYTE* buf, int nbytes );
	void DumpBytes( const CString& path, int nbytes );
	void DumpBits( const CString& path, int nbytes );

	~CBitstreamFile( void );

private:

	bool		AtByteBoundary()			{ return (m_bitidx < 0); }

	void		reset_buffers( void );
	CReturn		fill_cache( void );

	bool		ConditionalByteRead( BYTE* byte );

private:

	CString		m_filename;

	eFileMode	m_mode;
	CFile		m_file;		// File in process
	DWORD		m_flag;		// Various control flags, tbd

	int			m_fileidx;	// Number of bytes into file
	int			m_byteidx;	// Index within buffer; 0<=idx<len
	int			m_bitidx;
	
	int			m_filelen;	// Rotal length of file
	int			m_residual;	// Remaining length in file
	int			m_buflen;	// Length of buffer
	BYTE*		m_buf;		// File cache... my version

	bool		m_eof;		// TRUE if out of file...

	bool		m_usingTestBuffer;

	bool		m_bitCounter;
	int			m_bitsUsed;	// bits used since couter last activated
};

#endif
