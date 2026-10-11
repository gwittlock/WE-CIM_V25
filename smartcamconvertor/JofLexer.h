#ifndef _JOFLEXER_H
#define _JOFLEXER_H

#include <clex.h>

/* #line 8 "C:\\Work\\WE-CIM\\AdvMach\\smartcamconvertor\\JofLexer.l" */

class CJofParser;


/////////////////////////////////////////////////////////////////////////////
// CJofLexer

#ifndef YYDECLSPEC
#define YYDECLSPEC
#endif

class YYFAR YYDECLSPEC CJofLexer : public yyflexer {
public:
	CJofLexer();

protected:
	void yytables();
	virtual int yyaction(int action);

public:
/* #line 15 "C:\\Work\\WE-CIM\\AdvMach\\smartcamconvertor\\JofLexer.l" */

	// Operations
	public:
		BOOL	Init( CJofParser* in_parser );
		void	setString( const CString& in_str )	{ m_buf = in_str; m_buf_idx = 0; }

		char*	getToken( void )					{ return yytext; }

		// Attributes
	private:
		int yygetchar( void );

		CString	m_buf;
		int		m_buf_idx;

};

#ifndef YYLEXNAME
#define YYLEXNAME CJofLexer
#endif

#ifndef INITIAL
#define INITIAL 0
#endif

#endif
