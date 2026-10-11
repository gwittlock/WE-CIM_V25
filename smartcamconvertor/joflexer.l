
%{
#include "stdafx.h"
#include "JofParser.h"
%}


%include {
class CJofParser;

}


%name CJofLexer 
{
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
}
{
	// Constructor
}

%%

%{
// extract yylval for use later on in actions
YYSTYPE& yylval = *(YYSTYPE*)yyparserptr->yylvalptr;
%}


[ \t]					;

(\/{2,2}.*)				;

[=\[\]]						{ return yytext[0]; }

[-0-9]+						{ yylval.m_int = atoi(yytext); return INT; }

\".*\"[ \t]*$				{ 
								if ( strlen(yytext) < 3)
									yylval.m_string = "";
								else
								{
									yytext[ strlen(yytext)-1 ] = 0;
									yylval.m_string = strdup(yytext+1);	// Simply assigned to yytext+1 FAILS
								}
								return STRING;
							}
						
[a-z$#@A-Z_][a-zA-Z0-9_]*	{ 
								yylval.m_string = strdup(yytext);
								return IDENTIFIER;
							}

%%

// ============================================================================
// Init
//
BOOL
CJofLexer::Init( CJofParser* in_parser )
{
#ifdef YYDEBUG
	FILE* file;
	file = fopen( "JofLexer.log", "w" );
	yydebugout = file;
	yydebug = 1;

//	yydebugout = NULL;
//	yydebug = 0;
#endif

	return yycreate(in_parser);
}

int 
CJofLexer::yygetchar( void )
{
	if (m_buf_idx >= m_buf.GetLength())
		return EOF;
	return (int)m_buf[m_buf_idx++];
}
