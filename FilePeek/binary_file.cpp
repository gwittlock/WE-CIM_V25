// ==================================================================
//		Binary File
//
//		Generic Binary-oriented File Access Object
//
// Note the split techniques here:  CFile for reading, and fopen FILE
//	streams for writing.
//
// ==================================================================

#include "stdafx.h"
#include "cmn_resource.h"

#include "binary_file.h"

// ==================================================================


#define MAX_BINARY_FILE_BUF			1024
#define MAX_BINARY_FILE_CACHE		(10*MAX_BINARY_FILE_BUF)

// ==================================================================
//		CBinaryFile
//
CBinaryFile::CBinaryFile( void )
{
	m_mode = FILEMODE_ERROR;
	m_flag = 0;

	m_filelen	= 0;
	m_bufidx	= 0;
	m_buflen	= 0;
	m_buf		= NULL;
	m_stream	= NULL;

	m_eof = TRUE;
}
	
CBinaryFile::~CBinaryFile( void )
{
	reset_buffers();
}

// ==================================================================
//		reset_buffers
//
//		Set all internal values and buffers to empty
//		Only buffers -- doesn't affect File stuff.
//
void
CBinaryFile::reset_buffers( void )
{
	m_filelen	= 0;
	m_bufidx	= 0;
	m_buflen	= 0;
	if (m_buf) delete[] m_buf; m_buf = NULL;
}

// ==================================================================
//		Open
//
//		Open an existing file
//
CReturn
CBinaryFile::Open( 
	const CString&	in_name, 
	eFileMode		in_mode )
{
	CReturn			ret;
	CFileException	err;

	// Setup flags and junk, for easy open
	m_mode = in_mode;
	switch (in_mode)
	{
		case FILEMODE_READ:
			if (!m_file.Open( in_name, (CFile::modeRead), &err ))
				ret.SystemError( err.m_cause );
			break;
		case FILEMODE_WRITE:
			// Special write mode...
			m_stream = fopen( in_name, "wb" );
			if (!m_stream)
			{ ret.User( IDS_FILE_OPEN_ERR, in_name ); }
			break;

		case FILEMODE_APPEND:
		default:
			ret.setStatus( STATUS_ERROR );
			break;
	}
	if (!ret.isOkay())
		return ret;

	// If special case append, move to end of file.
	if (m_mode == FILEMODE_APPEND)
		m_file.SeekToEnd();

	// Now, some bit twiddling for setup
	reset_buffers();

	if (m_mode != FILEMODE_WRITE)
	{ m_filelen = m_file.GetLength(); }
	else
	{ m_filelen = 0; }

	m_eof = FALSE;

	m_filename = in_name;

	return ret;
}

// ==================================================================
//		Close
//
CReturn
CBinaryFile::Close( void )
{
	CReturn	ret;

	if (m_stream)
	{
		fclose( m_stream );
	}
	else
	{
		m_file.Close();
	}
	m_mode = FILEMODE_ERROR;
	m_eof = TRUE;

	// Don't reset here... keep latent data just in case.
	// WILL reset at a new Open, or at destruct.
	
	m_filename.Empty();

	return ret;
}

// ==================================================================
//		Read
//
//		Read a block of data
//
CReturn
CBinaryFile::Read( 
	BYTE*	buf,
	int		num )
{
	CReturn	ret;

	if (m_mode != FILEMODE_READ)
		return CReturn( STATUS_ERROR );

#define UNBUFFERED_READ 0
#if UNBUFFERED_READ
	m_file.Read( (LPVOID)buf, num );
#else
	// Suck a chunk of data out of the cache.
	int size = num;
	int chunk = 0;
	BYTE* dest = buf;

	while ((size>0) && ret.IsOk())
	{
		if ( (m_bufidx + size) > m_buflen )
			chunk = (m_buflen - m_bufidx);
		else
			chunk = size;

		if (chunk > 0)
		{
			memcpy( dest, &m_buf[m_bufidx], chunk );
			size -= chunk;

			m_bufidx += chunk;
			dest += chunk;
		}

		if (m_bufidx >= m_buflen)
			ret += fill_cache();
	}
#endif
	return ret;
}

// ==================================================================
//		Write
//
CReturn
CBinaryFile::Write( 
	BYTE*	buf,
	int		num )
{
	CReturn	ret;

	fwrite( (LPVOID)buf, 1, num, m_stream );

	if (ferror(m_stream))
	{ ret.SystemError( ferror(m_stream) ); }

	return ret;
}


// ==================================================================
//		fill_cache
//
CReturn
CBinaryFile::fill_cache( void )
{
	CReturn	ret;
	int		length;
	DWORD	got_length;

	// Failure traps
	if (m_filelen < 1)
		return CReturn( STATUS_ERROR );

	// How much to read?
	if (m_filelen <= MAX_BINARY_FILE_CACHE)
		length = m_filelen;
	else
		length = MAX_BINARY_FILE_CACHE;

	// Does this fit the current buffer?  It actually should...
	if (length > m_buflen)
	{
		if (m_buf) delete m_buf;
		m_buf = new BYTE[length];
	}
	m_buflen = length;	// reset length.
	m_filelen -= m_buflen;
	m_bufidx = 0;
	// Only the last cache read will be an odd length < MAX

	// Do the reading
	try
	{
		got_length = m_file.Read( (LPVOID)m_buf, m_buflen );

		if (m_buflen != (int)got_length)
		{
			m_buflen = got_length;
			m_filelen = 0;			// Error of sorts, stop looking
		}
	}
	catch( CFileException* e )
	{
		ret.Internal( e->m_cause );
		return ret;
	}

	return ret;
}

