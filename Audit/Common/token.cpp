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

#include "stdafx.h"
#include "token.h"


// ============================================================================
//		Token
//
CToken::CToken( void )
{
	m_flag = 0;
	m_break = 0;
}

CToken::~CToken( void )
{
}


// ============================================================================
//		addDelimit
//
//		Add a delimiter to the list, avoid repeats
//
void
CToken::addDelimit(
	char	in_delim )
{
	int	idx;

	idx = m_delimiter.Find( in_delim );
	if (idx < 0)
		m_delimiter += in_delim;
}

// ============================================================================
//		delDelimit
//
//		Delete a delimiter from the list
//
void
CToken::delDelimit( 
	char	in_delim )
{
	int	idx;

	idx = m_delimiter.Find( in_delim );
	if (idx >= 0)
	{
		CString	left;
		CString	right;

		if (idx > 0)
			left = m_delimiter.Left( idx );
		else
			left.Empty();

		idx = (m_delimiter.GetLength() - idx) - 1;
		if (idx > 0)
			right = m_delimiter.Right( idx );
		else
			right.Empty();

		m_delimiter = left;
		m_delimiter += right;
	}
}

// ============================================================================
//		getDelimit
//
//		Retrieve the value of a delimiter at a given index, 0 for fail
//
char
CToken::getDelimit( 
	int in_idx ) const
{
	if ( (in_idx < 0)
		|| (in_idx >= m_delimiter.GetLength()) )
	{
		return (char)0;
	}

	return m_delimiter[in_idx];
}


// ============================================================================
//		addMarker
//
//		Add a delimiter to the list, avoid repeats
//
void
CToken::addMarker(
	char	in_delim )
{
	int	idx;

	idx = m_marker.Find( in_delim );
	if (idx < 0)
		m_marker += in_delim;
}

// ============================================================================
//		delMarker
//
//		Delete a delimiter from the list
//
void
CToken::delMarker( 
	char	in_delim )
{
	int	idx;

	idx = m_marker.Find( in_delim );
	if (idx >= 0)
	{
		CString	left;
		CString	right;

		if (idx > 0)
			left = m_marker.Left( idx );
		else
			left.Empty();

		idx = (m_marker.GetLength() - idx) - 1;
		if (idx > 0)
			right = m_marker.Right( idx );
		else
			right.Empty();

		m_marker = left;
		m_marker += right;
	}
}

// ============================================================================
//		getMarker
//
//		Retrieve the value of a delimiter at a given index, 0 for fail
//
char
CToken::getMarker( 
	int in_idx ) const
{
	if ( (in_idx < 0)
		|| (in_idx >= m_marker.GetLength()) )
	{
		return (char)0;
	}

	return m_marker[in_idx];
}

// ============================================================================
//		getToken
//
//		Return a token from the current line, using the delimiter array
//		and break rules.
//
//		Returns an empty string when done.
//
CString
CToken::getToken( void )
{
	int		idx;
	bool	is_num;
	char	at;
	int		depth;

	m_token.Empty();
	if (m_line.IsEmpty())
		return m_token;

	depth = 0;

	// Determine character classificiation
	clrFlag( TOKEN_FLAG_CHAR | TOKEN_FLAG_STRING | TOKEN_FLAG_EXPRESS );
	
	// First strip leading Delimiters
	for (idx=0; idx<m_line.GetLength(); idx++)
	{
		if (m_delimiter.Find( m_line[idx] ) < 0)
			break;
	}
	if (idx)
		m_line = m_line.Right( m_line.GetLength() - idx );
	if (m_line.IsEmpty())
		return m_token;

	// Initial string check... must begin with it!
	is_num = tst_number( m_line[0] );
	if ( tstBreak( TOKEN_BREAK_STRINGS ) )
	{
		at = m_line[0];

		if (at == '\'')
			setFlag( TOKEN_FLAG_CHAR );
		else
		if (at == '"' )
			setFlag( TOKEN_FLAG_STRING );

		if ( isQuoted() )
			m_line = m_line.Right( m_line.GetLength() - 1 );
	}

	// Initial expression check... strings have priority
	if ( tstBreak( TOKEN_BREAK_EXPRESS ) 
		&& !isQuoted() )
	{
		at = m_line[0];

		if (at == '(')
			setFlag( TOKEN_FLAG_EXPRESS );
	}

	// Now scan out a token
	for (idx=0; idx<m_line.GetLength(); idx++)
	{
		at = m_line[idx];

		// If we are in a quoted arrangement, ONLY test for the end
		// of the quote.. and skip all other tests.
		if ( isChar() )
		{
			if (at == '\'')
				break;
			continue;
		}
		else
		if ( isString() )
		{
			if (at == '"')
				break;
			continue;
		}

		if (isExpress())
		{
			if (at == '(')
				depth++;
			else
			if (at == ')')
			{
				depth--;
				if (!depth)
				{
					idx++;
					break;
				}
			}
		}
		else
		{
			if (m_delimiter.Find( at ) >= 0)
			{
				setFlag( TOKEN_FLAG_DELIMIT );
				break;
			}

			if ( tstBreak( TOKEN_BREAK_ALNUM )
				&& (is_num != tst_number( at ) ) )
			{
				setFlag( TOKEN_FLAG_BREAK );
				break;
			}
			if ( tstBreak( TOKEN_BREAK_BRACKET )
				&& tst_bracket( at ) )
			{
				setFlag( TOKEN_FLAG_BREAK );
				break;
			}
			if ( tstBreak( TOKEN_BREAK_MARKER ) )
			{
				if (m_marker.Find( at ) >= 0)
				{
					setFlag( TOKEN_FLAG_MARK );
					break;
				}
			}
		}
	}
	if (!idx)
	{
		idx = 1;
		if (isQuoted())
			m_token.Empty();
		else
			m_token = m_line.Left( idx );
	}
	else
		m_token = m_line.Left( idx );

	// Skip trailing quote, if we were quoted!
	if ( isQuoted() )
		idx++;

	m_line = m_line.Right( m_line.GetLength() - idx );
	if (m_line.IsEmpty())
		setFlag( TOKEN_FLAG_EOL );

	return m_token;
}

// ============================================================================
//		putToken
//
//		Put a string at the head of the token line...
//
void
CToken::putToken( 
	const CString& in_tok )
{
	m_line = in_tok + m_line;
}

// ============================================================================
//		tst_number
//
//		Return TRUE if this character is a component of a number
//
bool
CToken::tst_number(
	char	in_tst ) const
{
	switch (in_tst)
	{
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case '-':
		case '.':
			return TRUE;
	}
	return FALSE;
}


// ============================================================================
//		tst_bracket
//
//		Return TRUE if this character is a bracket
//
bool
CToken::tst_bracket(
	char	in_tst ) const
{
	switch (in_tst)
	{
		case '(':
		case ')':
		case '[':
		case ']':
		case '{':
		case '}':
		case '<':
		case '>':
			return TRUE;
	}
	return FALSE;
}
