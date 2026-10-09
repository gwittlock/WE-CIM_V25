// ==================================================================
//		Binary File
//
//		Generic Binary-oriented File Access Object
//
// ==================================================================

#include "stdafx.h"

#include "Return.h"
#include "BitstreamFile.h"

// ==================================================================


#define MAX_BINARY_FILE_BUF			1024
#define MAX_BINARY_FILE_CACHE		(10*MAX_BINARY_FILE_BUF)

BYTE Mask( int bit );


// ==================================================================
//		CBitstreamFile
//
CBitstreamFile::CBitstreamFile( void )
{
	m_mode = FILEMODE_ERROR;
	m_flag = 0;

	m_residual	=  0;
	m_fileidx	=  0;
	m_byteidx	= -1;
	m_bitidx	= -1;

	m_buflen	=  0;
	m_buf		= NULL;

	m_eof = TRUE;

	m_usingTestBuffer = FALSE;

	m_bitCounter = FALSE;
	m_bitsUsed = 0;
}
	
CBitstreamFile::~CBitstreamFile( void )
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
CBitstreamFile::reset_buffers( void )
{
	m_residual	=  0;
	m_byteidx	= -1;
	m_bitidx	= -1;
	m_buflen	=  0;
	if (m_buf) delete m_buf; m_buf = NULL;
}

// ==================================================================
//		Open
//
//		Open an existing file
//
CReturn
CBitstreamFile::Open( 
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
			//ret.Internal( err.m_cause );
		break;
	case FILEMODE_WRITE:
		if (!m_file.Open( in_name,
							CFile::modeCreate 
							| CFile::modeWrite,
							&err) )
			ret.SystemError( err.m_cause );
			//ret.Internal( err.m_cause );
		break;

	case FILEMODE_APPEND:
		if (!m_file.Open( in_name, 
							CFile::modeCreate
							| CFile::modeWrite
							| CFile::modeNoTruncate,
							&err))
		{
			ret.SystemError( err.m_cause );
			//ret.Internal( err.m_cause );
		}
		break;

	default:
		// TODO: error message?
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
	m_residual = Length();

	m_eof = FALSE;

	m_filename = in_name;

	return ret;
}

// ==================================================================
//		Close
//
CReturn
CBitstreamFile::Close( void )
{
	CReturn	ret;

	m_file.Close();
	m_mode = FILEMODE_ERROR;
	m_eof = TRUE;

	// Don't reset here... keep latent data just in case.
	// WILL reset at a new Open, or at destruct.
	
	m_filename.Empty();

	return ret;
}

long
CBitstreamFile::Length()
{
	m_filelen =(int) m_file.GetLength();
	return m_filelen;
}

// ==================================================================
//		Seek
//
//		where from is one of (CFile::begin, CFile::current, CFile::end)
//
CReturn
CBitstreamFile::Seek( LONG lOff, UINT from )
{
	CReturn status;
	long length;

	reset_buffers();

	length = Length();
	m_residual = length - lOff;

	m_eof = FALSE;

	m_file.Seek( lOff, from );	// TODO: exception handling!
	status = fill_cache();

	m_fileidx = lOff;

	return status;
}


// Seek relative to the beginning of the internal buffer.
CReturn
CBitstreamFile::BufferSeek( int bytes, int bits )
{
	CReturn	status;

	// if ((bytes < 0) || (bytes >= m_buflen) ||
		// (bits < 0) || (bits >= 8) )
	//if ((bytes < 0) || (bytes >= m_buflen))
	//{
	//	status.SystemError();
	//}
	//else
	{
		m_byteidx = bytes;
		m_bitidx = bits;
	}

	return status;
}

void
CBitstreamFile::SyncByte()
{
	// A bitidx of 0 means we are on a byte boundary anyway
	// A bitidx of -1 means we are off the bit stream entirely
	// Any other bitidx means we need to skip ahead... by marking it
	//	at -1, we discard the current byte and step out of the bitstream
	//
	//if (m_bitidx != 0)
	if (m_bitidx > -1)
	{
		m_bitidx = -1;
	}
}

// ==================================================================
//		Read
//
//		Read a block of data
//
CReturn
CBitstreamFile::ReadBytes( BYTE* buf, int num )
{
	return ReadBits( buf, (num * 8) );
}

CReturn
CBitstreamFile::ReadByte( BYTE* buf )
{
	return ReadBits( buf, 8 );
}

// NOTE:
// As the caller, you must initialize your buffer when:
//
// 1. you are casting a data type whose size is greater than one byte AND
//    the number of requested nbits will not affect the upper bytes.
//
//    eg.
//        long foo;
//        ReadBits( (BYTE*) &long, 2 );  // WRONG! yields unexpected results
//
//        long foo = 0;
//        ReadBits( (BYTE*) &long, 2 );  // RIGHT!
//
CReturn
CBitstreamFile::ReadBits( BYTE* buf, int nbits )
{
	CReturn status;

	BYTE*	byte;
	BYTE	mask;
	BYTE	srcbyte;
	BYTE	bit;
	int		nbytes;
	int		indx;
	int		nbitsUsed;
	bool	isMult8;
	bool	isByteAligned;

	if ( m_bitCounter )
		m_bitsUsed += nbits;

	// Is the requested number of bits a multiple of 8?
	isMult8 = ((nbits % 8) == 0);
	isByteAligned = (isMult8 && AtByteBoundary());

	// Determine how many bytes will be output.
	nbytes = (nbits / 8);
	if ( !isMult8 )
		++nbytes;

	nbitsUsed = 0;

	// Set up the first recipient byte.
	indx = 0;
	byte = &(buf[indx]);
	*byte = 0;

	if ( !ConditionalByteRead( &srcbyte ) )
	{
		status.SystemError();
		return status;
	}

	// The local variable 'mask' is used to save function calls to Mask().
	mask = Mask( m_bitidx );

	// While not done processing the bits ...
	while (nbits > 0)
	{
		if ( isByteAligned )
		{
			(*byte) = srcbyte;

			m_bitidx = -1;

			nbitsUsed = 8;
			nbits -= 8;
		}
		else
		{
			bit = ((srcbyte & mask) ? 0x01 : 0x00);

			(*byte) <<= 1;
			(*byte) |= bit;

			--m_bitidx;
			if ( AtByteBoundary() )
			{
				mask = Mask( 7 );
			}
			else
			{
				mask >>= 1;
			}

			++nbitsUsed;
			--nbits;
		}

		if ( AtByteBoundary() )
		{
			if (nbits > 0)
			{
				if ( !ConditionalByteRead( &srcbyte ) )
				{
					status.SystemError();
					return status;
				}
			}
		}

		if (nbitsUsed == 8)
		{
			nbitsUsed = 0;

			if ((indx + 1) < nbytes)
			{
				// Set up the next recipient byte as necessary.
				++indx;
				byte = &(buf[indx]);
				*byte = 0;
			}
		}
	}

	return status;
}

CReturn
CBitstreamFile::ReadBit( BYTE* buf )
{
	return ReadBits( buf, 1 );
}

// ==================================================================
//		ReadWord
//
//		Read one word, shortcut
//
CReturn
CBitstreamFile::ReadWord( WORD* buf )
{
	return ReadBytes( (BYTE*) buf, 2 );
}


// ==================================================================
//		Write
//
CReturn
CBitstreamFile::WriteBytes( 
	BYTE*	buf,
	int		num )
{
	CReturn	ret;

	return ret;
}

void
CBitstreamFile::BitCounter( bool on )
{
	m_bitCounter = on;
	m_bitsUsed = 0;
}

void
CBitstreamFile::TestBufferSet( BYTE* buf, int nbytes )
{
	if (m_buf) delete m_buf;
	
	m_buf = new BYTE[nbytes];
	memcpy( m_buf, buf, nbytes );

	m_buflen = nbytes;
	m_residual = nbytes;

	m_byteidx = -1;
	m_bitidx = -1;

	m_mode = FILEMODE_READ;

	m_usingTestBuffer = TRUE;
}

void
CBitstreamFile::DumpBytes( const CString& path, int nbytes )
{
	FILE* f;

	f = fopen( path, "wb" );
	if (f != NULL)
	{
		fwrite( m_buf, sizeof( BYTE ), nbytes, f );
		fflush( f );
		fclose( f );
	}
}

void
CBitstreamFile::DumpBits( const CString& path, int nbytes )
{
	FILE* f;
	BYTE byte;
	int indx;

	f = fopen( path, "w" );
	if (f != NULL)
	{
		for (indx = 0; indx < nbytes; ++indx)
		{
			byte = m_buf[m_byteidx+indx+1];

			fprintf( f, "%d", ((byte & 0x80) ? 1 : 0) );
			fprintf( f, "%d", ((byte & 0x40) ? 1 : 0) );
			fprintf( f, "%d", ((byte & 0x20) ? 1 : 0) );
			fprintf( f, "%d", ((byte & 0x10) ? 1 : 0) );
			fprintf( f, " " );
			fprintf( f, "%d", ((byte & 0x08) ? 1 : 0) );
			fprintf( f, "%d", ((byte & 0x04) ? 1 : 0) );
			fprintf( f, "%d", ((byte & 0x02) ? 1 : 0) );
			fprintf( f, "%d", ((byte & 0x01) ? 1 : 0) );
			fprintf( f, " " );

			if (((indx + 1) % 8) == 0)
				fprintf( f, "\n\n" );
		}
		fflush( f );
		fclose( f );
	}
}

// ==================================================================
//		fill_cache
//
CReturn
CBitstreamFile::fill_cache( void )
{
	CReturn	ret;
	int		length;
	DWORD	got_length;

	if ( m_usingTestBuffer )
	{
		return ret;
	}

	// Failure traps
	if (m_residual < 1)
		return CReturn( STATUS_ERROR );

	// How much to read?
	if (m_residual <= MAX_BINARY_FILE_CACHE)
		length = m_residual;
	else
		length = MAX_BINARY_FILE_CACHE;

	// Does this fit the current buffer?  It actually should...
	if (length > m_buflen)
	{
		if (m_buf) delete m_buf;
		m_buf = new BYTE[length];
	}

	m_buflen = length;	// reset length.
	m_residual -= m_buflen;

	m_byteidx = -1;
	m_bitidx = -1;

	// Only the last cache read will be an odd length < MAX

	// Do the reading
	try
	{
		got_length = m_file.Read( (LPVOID)m_buf, m_buflen );
		if (m_buflen != (int)got_length)
		{
			m_buflen = got_length;
			m_residual = 0;			// Error of sorts, stop looking
		}
	}
	catch( CFileException* e )
	{
		ret.Internal( e->m_cause );
		return ret;
	}

	return ret;
}

bool
CBitstreamFile::ConditionalByteRead( BYTE* byte )
{
	bool okay = TRUE;

	if (m_byteidx >= m_buflen)
	{
		// Subtract one because... because... it has to.
		okay = ( Seek( m_fileidx + m_buflen - 1, CFile::begin ).IsOk() );
//		okay = ( fill_cache().IsOk() );
	}

	if ( okay )
	{
		if (m_bitidx < 0)
		{
			// We've past the byte boundary.
			++m_byteidx;
			m_bitidx = 7;
		}

		(*byte) = m_buf[m_byteidx];
	}

	return okay;
}

BYTE
Mask( int bit )
{
	switch( bit )
	{
	case 7:		return 0x80;
	case 6:		return 0x40;
	case 5:		return 0x20;
	case 4:		return 0x10;
	case 3:		return 0x08;
	case 2:		return 0x04;
	case 1:		return 0x02;
	case 0:		return 0x01;
	default:	return 0x00;
	}
}