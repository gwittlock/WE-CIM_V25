#ifndef _TOKEN_H
#define _TOKEN_H

// ============================================================================
//		Token
//
//		Token processor.  Receives a line, and returns individual tokens
//		from it.  Based on numerous extent functions, wrapped for handy
//		re-use.
//
//		Note that there are two types of parsing rule:
//			Delimeters seperate tokens, and are discarded
//
//			Breaks are flags that indicate conditions or text that end a token,
//				but any breaking value is kept for the next token
//
// ============================================================================

#include <afx.h>
#include <afxtempl.h>
#include <afxcoll.h>

#include "text_file.h"
#include "Return.h"

// ============================================================================
//		Break flags

#define TOKEN_BREAK_ALNUM	0x01		// Break at alpha/num boundary
#define TOKEN_BREAK_STRINGS	0x02		// Allow single and double quoted string extraction
#define TOKEN_BREAK_MARKER	0x04		// Break based on marker list
#define TOKEN_BREAK_BRACKET	0x08		// Break on brackets
#define TOKEN_BREAK_EXPRESS	0x10		// Allow parenthesis bracketed expressions (don't use BREAK_BRACKET)

// ============================================================================
//		Status flags

#define TOKEN_FLAG_CHAR			0x0001		// Token was a quoted character
#define TOKEN_FLAG_STRING		0x0002		// Token was a quoted string
#define TOKEN_FLAG_DELIMIT		0x0004		// Token stopped by delimeter
#define TOKEN_FLAG_MARK			0x0008		// Token stopped by a marker
#define TOKEN_FLAG_BREAK		0x0010		// Token stopped by other break condition
#define TOKEN_FLAG_EOL			0x0020		// Token stopped by end of line
#define TOKEN_FLAG_POUND		0x0040		// Use the '#' marker for numbered tokens
#define TOKEN_FLAG_STAR			0x0080		// Try the '*' marker for unknown tokens
#define TOKEN_FLAG_EXPRESS		0x0100		// Is a bracketed expression

// ============================================================================

class dllExport CToken
{
public:
	CToken( void );
	~CToken( void );

	CReturn	Open( const CString& in_name, 
					eFileMode in_mode=FILEMODE_READ )	{ return m_textfile.Open( in_name, in_mode ); }
	CReturn	Close( void )								{ return m_textfile.Close(); }

	CString	ReadLine( void )							{ return m_textfile.ReadLine(); }
	CReturn	WriteLine( const CString& in_line )			{ return m_textfile.WriteLine( in_line ); }
	CReturn	Write( const CString& in_text )				{ return m_textfile.Write( in_text ); }

	int		getLineNum( void ) const					{ return m_textfile.getLineNum(); }
	bool	isEOF( void ) const							{ return m_textfile.isEOF(); }

	// -------------------------------

	void	setLine( const CString& in_line )			{ m_line = in_line; }
	CString	getLine( void ) const						{ return m_line; }

	void	clrDelimit( void )							{ m_delimiter.Empty(); }
	void	addDelimit( char in_delim );
	void	delDelimit( char in_delim );
	char	getDelimit( int in_idx ) const;

	void	clrMarker( void )								{ m_marker.Empty(); }
	void	addMarker( char in_delim );
	void	delMarker( char in_delim );
	char	getMarker( int in_idx ) const;

	void	clrBreak( int in_break )					{ m_break &= ~in_break; }
	void	setBreak( int in_break )					{ m_break |= in_break; }
	int		tstBreak( int in_break ) const				{ return m_break & in_break; }

	void	clrFlag( int in_flag )						{ m_flag &= ~in_flag; }
	void	setFlag( int in_flag )						{ m_flag |= in_flag; }
	int		tstFlag( int in_flag )						{ return m_flag & in_flag; }
	bool	isQuoted( void ) const						{ return ((m_flag & (TOKEN_FLAG_CHAR | TOKEN_FLAG_STRING)) != 0); }
	bool	isString( void ) const						{ return ((m_flag & TOKEN_FLAG_STRING) != 0); }
	bool	isChar( void ) const						{ return ((m_flag & TOKEN_FLAG_CHAR) != 0); }
	bool	isExpress( void ) const						{ return ((m_flag & TOKEN_FLAG_EXPRESS) != 0); }

	CString	getToken( void );
	void	putToken( const CString& in_tok );

protected:
	bool		tst_number( char in_tst ) const;
	bool		tst_bracket( char in_tst ) const;

	CTextFile	m_textfile;

	CString		m_line;
	CString		m_token;
	int			m_flag;

	int			m_break;			// Break control flags
	CString		m_marker;			// Optional break markers
	CString		m_delimiter;		// Required token delimiters
};

#endif