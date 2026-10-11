#ifndef _JOFPARSER_H
#define _JOFPARSER_H

#include <cyacc.h>

/* #line 10 "C:\\Work\\WE-CIM\\AdvMach\\smartcamconvertor\\JofParser.y" */

#include "JofLexer.h"
#include "Jof.h"

/////////////////////////////////////////////////////////////////////////////
// CJofParser

#ifndef YYDECLSPEC
#define YYDECLSPEC
#endif

class YYFAR YYDECLSPEC CJofParser : public yyfparser {
public:
	CJofParser();

protected:
	void yytables();
	virtual void yyaction(int action);
#ifdef YYDEBUG
	void YYFAR* yyattribute1(int index) const;
	void yyinitdebug(void YYFAR** p, int count) const;
#endif

public:
/* #line 16 "C:\\Work\\WE-CIM\\AdvMach\\smartcamconvertor\\JofParser.y" */

	// Operations
	public:
		BOOL Init( CJofLexer* in_lexer, CJof* jof );

	// Attributes
	private:
		void yyerror( const char* in_error );

		CJofLexer*	m_lexer;
		CJof*		m_jof;

};

#ifndef YYPARSENAME
#define YYPARSENAME CJofParser
#endif

#ifndef YYSTYPE
union tagYYSTYPE {
/* #line 33 "C:\\Work\\WE-CIM\\AdvMach\\smartcamconvertor\\JofParser.y" */

	int				m_int;
	char*			m_string;

};

#define YYSTYPE union tagYYSTYPE
#endif

#define IDENTIFIER 257
#define STRING 258
#define INT 259
#endif
