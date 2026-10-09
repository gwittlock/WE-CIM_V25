#define IDENTIFIER 257
#define STRING 258
#define INT 259
#define REAL 260
#ifdef CMD_YYSTYPE
#undef  CMD_YYSTYPE_IS_DECLARED
#define CMD_YYSTYPE_IS_DECLARED 1
#endif
#ifndef CMD_YYSTYPE_IS_DECLARED
#define CMD_YYSTYPE_IS_DECLARED 1
typedef union
{
	void*	m_void;
	int		m_int;
	double	m_real;
/*	char	m_string[256];  // used char[] to avoid memory management issues!*/
	char	m_string[1024];  /* used char[] to avoid memory management issues!*/
} CMD_YYSTYPE;
#endif /* !CMD_YYSTYPE_IS_DECLARED */
extern CMD_YYSTYPE cmd_yylval;
