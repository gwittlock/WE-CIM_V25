// ==================================================================
//		Text File
//
//		Generic Text-oriented File Access Object
//
// ==================================================================

#include "stdafx.h"
#include "text_file.h"

// ==================================================================

#define MAX_TEXT_FILE_LINE		1024
#define MAX_TEXT_FILE_CACHE		1024

// ==================================================================
//		CTextFile
//
CTextFile::CTextFile( void )
{
	m_mode = FILEMODE_ERROR;
	m_flag = 0;

	m_filelen	= 0;
	m_bufidx	= 0;
	m_buflen	= 0;
	m_buf		= NULL;

	m_eof = TRUE;

	m_lineno = 0;
}
	
CTextFile::~CTextFile( void )
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
CTextFile::reset_buffers( void )
{
	m_filelen	= 0;
	m_bufidx	= 0;
	m_buflen	= 0;
	if (m_buf) delete m_buf; m_buf = NULL;
}

// ==================================================================
//		Open
//
//		Open an existing file
//
CReturn
CTextFile::Open( 
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
	m_filelen = m_file.GetLength();

	m_eof = FALSE;
	m_lineno = 0;

	m_filename = in_name;

	return ret;
}

// ==================================================================
//		Close
//
CReturn
CTextFile::Close( void )
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

// ==================================================================
//		ReadLine
//
//		Read a line of text, terminating at a CR/LF combo or single
//		Should be able to handle DOS or Unix termination
//
//		Ignores NULL values, and treats them like real text
//
//		Entirely skips blank lines (part of eol skipping).
//
static char global_text_line[MAX_TEXT_FILE_LINE];

#define STATE_BODY	0
#define STATE_CR	1
#define STATE_LF	2
#define STATE_EXIT	3

CString
CTextFile::ReadLine( void )
{
	char*	to;
	int		cnt;
	int		state;

	if (m_mode != FILEMODE_READ)
		return CString("");

	// Suck a line of text out of the cache.
	// Lines are terminated by 0x0a, 0x0c, 0x0d;
	//	if find one of these, skip any others, then done.
	cnt=0;
	state = STATE_BODY;

	to = global_text_line;
	while (state != STATE_EXIT)
	{
		// If out of buffer...
		if (m_bufidx >= m_buflen)
		{
			// ... read more into the cache...
			if (!fill_cache().isOkay())
			{
				if (!cnt)
				{
					m_eof = TRUE;
					return CString("");
				}

				*to = 0;
				return CString(global_text_line);
			}
		}

		switch (state)
		{
			case STATE_BODY:
				if (m_buf[m_bufidx] == 0x0a)
					state = STATE_LF;
				else
				if (m_buf[m_bufidx] == 0x0d)
					state = STATE_CR;
				else
				{
					if (m_buf[m_bufidx] == 0)
					{
						// Convert NULL into space...
						*to++ = ' ';
					}
					else
						*to++ = m_buf[m_bufidx];

					cnt++;
					if (cnt >= MAX_TEXT_FILE_LINE)
						state = STATE_EXIT;
				}
				m_bufidx++;
				break;

			case STATE_CR:
				if (m_buf[m_bufidx] == 0x0a)
					m_bufidx++;
				state = STATE_EXIT;
				break;

			case STATE_LF:
				if (m_buf[m_bufidx] == 0x0d)
					m_bufidx++;
				state = STATE_EXIT;
				break;
		}
	}

	if (!cnt)
		return CString("");

	*to = 0;
	m_lineno++;
	return CString(global_text_line);
}

// ==================================================================
//		WriteLine
//
//		Write a line of text out to the file.  Terminates it with
//		DOS CR/LF.  No need to add termination data to the input
//		line.
//
CReturn
CTextFile::WriteLine( 
	CString in_line )
{
	CReturn	ret;

	if ( (m_mode != FILEMODE_WRITE)
		  && (m_mode != FILEMODE_APPEND) )
	{
		return CReturn( STATUS_ERROR );
	}

	ret += Write( in_line );
	if (ret.isOkay())
	{
		ret += Write( "\x00d\x00a" );
		if (ret.isOkay())
		{
			m_lineno++;
		}
	}
	
	return ret;
}

// ==================================================================
//		Write
//
//		Write text out to the file
//
CReturn
CTextFile::Write( 
	CString in_line )
{
	CReturn	ret;

	if ( (m_mode != FILEMODE_WRITE)
		  && (m_mode != FILEMODE_APPEND) )
	{
		return CReturn( STATUS_ERROR );
	}

	try
	{
		m_file.Write( (void*)(LPCSTR)in_line, in_line.GetLength() );
	}
	catch( CFileException* e )
	{
		ret.Internal( e->m_cause );
		return ret;
	}

	return ret;
}


// ==================================================================
//		fill_cache
//
//		Fill the file cache with more data... or not
//		This improves speed a *lot* with text files... because for
//		text you must scan one character at a time, which is very
//		slow through the standard Windows interface.
//
CReturn
CTextFile::fill_cache( void )
{
	CReturn	ret;
	int		length;
	DWORD	got_length;

	// Failure traps
	if (m_filelen < 1)
		return CReturn( STATUS_ERROR );

	// How much to read?
	if (m_filelen <= MAX_TEXT_FILE_CACHE)
		length = m_filelen;
	else
		length = MAX_TEXT_FILE_CACHE;

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

