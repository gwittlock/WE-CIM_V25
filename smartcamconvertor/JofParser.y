
%{
#include "stdafx.h"
#include "cmn_resource.h"
#include "Return.h"

%}

%include
{
#include "JofLexer.h"
#include "Jof.h"
}

%name CJofParser
{
	// Operations
	public:
		BOOL Init( CJofLexer* in_lexer, CJof* jof );

	// Attributes
	private:
		void yyerror( const char* in_error );

		CJofLexer*	m_lexer;
		CJof*		m_jof;
}
{
	// Constructor
}

%union
{
	int				m_int;
	char*			m_string;
}

%token <m_string>	IDENTIFIER
%token <m_string>	STRING
%token <m_int>		INT

%%

// ============================================================================
//		command
//
//
jof: plos
	;
	
plos:
	| plos plo
	;

plo:
	IDENTIFIER '=' INT '[' nvus ']'		{ 
											m_jof->PloCreate( $1, $3 );
											
											free( $1 );
											$1 = NULL;
										}
	;

nvus:
	| nvus nvu
	;

nvu:
	IDENTIFIER '=' STRING				{
											m_jof->NvuAdd( $1, $3 );
											
											free( $1 );
											$1 = NULL;
											free( $3 );
											$3 = NULL;
										}
	;

%%

// ============================================================================

void
CJofParser::yyerror( const char* in_error )
{
	CReturn	msg;

	msg.Internal( IDS_PARSE_SYNTAX, in_error, m_lexer->getToken() );
}

// ============================================================================
// Init
//
BOOL
CJofParser::Init( 
	CJofLexer*	in_lexer,
	CJof*		jof )
{
	m_lexer = in_lexer;
	m_jof = jof;

	if (!yycreate(in_lexer))
		return FALSE;

#ifdef YYDEBUG
	FILE* file;
	file = fopen( "JofParser.log", "w" );
	yydebugout = file;
	yydebug = 1;

//	yydebugout = NULL;
//	yydebug = 0;
#endif

	return TRUE;
}
