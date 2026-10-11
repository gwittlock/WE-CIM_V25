#include <clex.h>

/* #line 2 "C:\\Work\\WE-CIM\\AdvMach\\smartcamconvertor\\JofLexer.l" */

#include "stdafx.h"
#include "JofParser.h"

// repeated because of possible precompiled header
#include <clex.h>

#include "JofLexer.h"

/////////////////////////////////////////////////////////////////////////////
// constructor

YYLEXNAME::YYLEXNAME()
{
	yytables();
/* #line 30 "C:\\Work\\WE-CIM\\AdvMach\\smartcamconvertor\\JofLexer.l" */

	// Constructor

}

#ifndef YYTEXT_SIZE
#define YYTEXT_SIZE 100
#endif
#ifndef YYUNPUT_SIZE
#define YYUNPUT_SIZE YYTEXT_SIZE
#endif

// backwards compatability with lex
#ifdef input
int YYLEXNAME::yyinput()
{
	return input();
}
#else
#define input yyinput
#endif

#ifdef output
void YYLEXNAME::yyoutput(int ch)
{
	output(ch);
}
#else
#define output yyoutput
#endif

#ifdef unput
void YYLEXNAME::yyunput(int ch)
{
	unput(ch);
}
#else
#define unput yyunput
#endif

#ifndef YYNBORLANDWARN
#ifdef __BORLANDC__
#pragma warn -rch		// <warning: unreachable code> off
#endif
#endif

int YYLEXNAME::yyaction(int action)
{
/* #line 36 "C:\\Work\\WE-CIM\\AdvMach\\smartcamconvertor\\JofLexer.l" */

// extract yylval for use later on in actions
YYSTYPE& yylval = *(YYSTYPE*)yyparserptr->yylvalptr;

	yyreturnflg = 1;
	switch (action) {
	case 1:
		{
/* #line 42 "C:\\Work\\WE-CIM\\AdvMach\\smartcamconvertor\\JofLexer.l" */
;
		}
		break;
	case 2:
		{
/* #line 44 "C:\\Work\\WE-CIM\\AdvMach\\smartcamconvertor\\JofLexer.l" */
;
		}
		break;
	case 3:
		{
/* #line 46 "C:\\Work\\WE-CIM\\AdvMach\\smartcamconvertor\\JofLexer.l" */
 return yytext[0]; 
		}
		break;
	case 4:
		{
/* #line 48 "C:\\Work\\WE-CIM\\AdvMach\\smartcamconvertor\\JofLexer.l" */
 yylval.m_int = atoi(yytext); return INT; 
		}
		break;
	case 5:
		{
/* #line 50 "C:\\Work\\WE-CIM\\AdvMach\\smartcamconvertor\\JofLexer.l" */
 
								if ( strlen(yytext) < 3)
									yylval.m_string = "";
								else
								{
									yytext[ strlen(yytext)-1 ] = 0;
									yylval.m_string = strdup(yytext+1);	// Simply assigned to yytext+1 FAILS
								}
								return STRING;
							
		}
		break;
/* #line 60 "C:\\Work\\WE-CIM\\AdvMach\\smartcamconvertor\\JofLexer.l" */
						
	case 6:
		{
/* #line 61 "C:\\Work\\WE-CIM\\AdvMach\\smartcamconvertor\\JofLexer.l" */
 
								yylval.m_string = strdup(yytext);
								return IDENTIFIER;
							
		}
		break;
	default:
		yyassert(0);
		break;
	}
	yyreturnflg = 0;
	return 0;
}

#ifndef YYNBORLANDWARN
#ifdef __BORLANDC__
#pragma warn .rch		// <warning: unreachable code> to the old state
#endif
#endif
/* #line 66 "C:\\Work\\WE-CIM\\AdvMach\\smartcamconvertor\\JofLexer.l" */


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


void YYLEXNAME::yytables()
{
	yystext_size = YYTEXT_SIZE;
	yysunput_size = YYUNPUT_SIZE;

	static const yymatch_t YYNEARFAR YYBASED_CODE match[] = {
		0
	};
	yymatch = match;

	yytransitionmax = 153;
	static const yytransition_t YYNEARFAR YYBASED_CODE transition[] = {
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 3, 1 },
		{ 0, 9 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 3, 1 },
		{ 0, 0 },
		{ 7, 1 },
		{ 8, 1 },
		{ 8, 1 },
		{ 10, 7 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 6, 1 },
		{ 0, 0 },
		{ 4, 1 },
		{ 6, 1 },
		{ 6, 1 },
		{ 6, 1 },
		{ 6, 1 },
		{ 6, 1 },
		{ 6, 1 },
		{ 6, 1 },
		{ 6, 1 },
		{ 6, 1 },
		{ 6, 1 },
		{ 9, 4 },
		{ 0, 0 },
		{ 0, 0 },
		{ 5, 1 },
		{ 6, 6 },
		{ 0, 0 },
		{ 8, 1 },
		{ 6, 6 },
		{ 6, 6 },
		{ 6, 6 },
		{ 6, 6 },
		{ 6, 6 },
		{ 6, 6 },
		{ 6, 6 },
		{ 6, 6 },
		{ 6, 6 },
		{ 6, 6 },
		{ 0, 0 },
		{ 0, 0 },
		{ 8, 8 },
		{ 8, 8 },
		{ 8, 8 },
		{ 8, 8 },
		{ 8, 8 },
		{ 8, 8 },
		{ 8, 8 },
		{ 8, 8 },
		{ 8, 8 },
		{ 8, 8 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 5, 1 },
		{ 0, 0 },
		{ 5, 1 },
		{ 8, 8 },
		{ 8, 8 },
		{ 8, 8 },
		{ 8, 8 },
		{ 8, 8 },
		{ 8, 8 },
		{ 8, 8 },
		{ 8, 8 },
		{ 8, 8 },
		{ 8, 8 },
		{ 8, 8 },
		{ 8, 8 },
		{ 8, 8 },
		{ 8, 8 },
		{ 8, 8 },
		{ 8, 8 },
		{ 8, 8 },
		{ 8, 8 },
		{ 8, 8 },
		{ 8, 8 },
		{ 8, 8 },
		{ 8, 8 },
		{ 8, 8 },
		{ 8, 8 },
		{ 8, 8 },
		{ 8, 8 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 8, 8 },
		{ 0, 0 },
		{ 8, 8 },
		{ 8, 8 },
		{ 8, 8 },
		{ 8, 8 },
		{ 8, 8 },
		{ 8, 8 },
		{ 8, 8 },
		{ 8, 8 },
		{ 8, 8 },
		{ 8, 8 },
		{ 8, 8 },
		{ 8, 8 },
		{ 8, 8 },
		{ 8, 8 },
		{ 8, 8 },
		{ 8, 8 },
		{ 8, 8 },
		{ 8, 8 },
		{ 8, 8 },
		{ 8, 8 },
		{ 8, 8 },
		{ 8, 8 },
		{ 8, 8 },
		{ 8, 8 },
		{ 8, 8 },
		{ 8, 8 }
	};
	yytransition = transition;

	static const yystate_t YYNEARFAR YYBASED_CODE state[] = {
		{ 0, 0, 0 },
		{ 8, 1, 0 },
		{ 1, 0, 0 },
		{ 0, 0, 1 },
		{ 0, 12, 0 },
		{ 0, 0, 3 },
		{ 0, 18, 4 },
		{ -7, 4, 0 },
		{ 0, 30, 6 },
		{ -9, 1, 2 },
		{ 0, 0, 5 }
	};
	yystate = state;

	static const yybackup_t YYNEARFAR YYBASED_CODE backup[] = {
		0,
		0,
		0,
		0,
		0,
		0,
		0
	};
	yybackup = backup;
}
