#define DECINT 257
#define HEXINT 258
#define REAL 259
#define OP_EQ 260
#define OP_LE 261
#define OP_GE 262
#define OP_LT 263
#define OP_GT 264
#define OP_NE 265
#define OP_AND 266
#define OP_OR 267
#define OP_XOR 268
#define OP_MIN 269
#define OP_MAX 270
#define OP_LSHIFT 271
#define OP_RSHIFT 272
#define OP_SIN 273
#define OP_COS 274
#define OP_TAN 275
#define OP_ASIN 276
#define OP_ACOS 277
#define OP_ATAN 278
#define OP_RAD 279
#define OP_DEG 280
#define OP_NOT 281
#define UMINUS 282
#ifdef EXP_YYSTYPE
#undef  EXP_YYSTYPE_IS_DECLARED
#define EXP_YYSTYPE_IS_DECLARED 1
#endif
#ifndef EXP_YYSTYPE_IS_DECLARED
#define EXP_YYSTYPE_IS_DECLARED 1
typedef union
{
	int		m_int;
	double	m_real;
} EXP_YYSTYPE;
#endif /* !EXP_YYSTYPE_IS_DECLARED */
extern EXP_YYSTYPE exp_yylval;
