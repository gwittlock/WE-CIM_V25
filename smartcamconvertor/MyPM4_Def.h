#ifndef _MYPM4_DEF_H
#define _MYPM4_DEF_H

// =======================================================================
//		File-Format definitions...
//
//
// Common block definition
//

#define COM_OFF_FLAG0		0		// BYTE	flag0
#define COM_OFF_TAG			1		// BYTE	tag
#define COM_OFF_PLANE		2		// WORD	ref plane
#define COM_OFF_OFFSET		4		// WORD	offset
#define COM_OFF_DUMMY0		6		// BYTE	dummy0
#define COM_OFF_DUMMY1		7		// BYTE	dummy1
#define COM_OFF_TOOL		8		// WORD	tool
#define COM_OFF_GROUP		10		// WORD	Group flags + tool flag
#define COM_OFF_MAT_TOP		12		// float	prof_top (check z)
#define COM_OFF_CLEAR_Z		16		// float	clear_z

#define PM4_TOOL_GROUP		0x8000	// Set in tool if tool

// -------------------------------------

#define POINT_OFF_X		20
#define POINT_OFF_Y		28
#define POINT_OFF_Z		36

// -------------------------------------

#define LINE_OFF_EX	20
#define LINE_OFF_EY	28
#define LINE_OFF_EZ	36
	
#define LINE_OFF_SX	44
#define LINE_OFF_SY	52
#define LINE_OFF_SZ	60

// -------------------------------------

#define ARC_OFF_EX	20
#define ARC_OFF_EY	28
#define ARC_OFF_EZ	36

#define ARC_OFF_SX	44
#define ARC_OFF_SY	52
#define ARC_OFF_SZ	60

#define ARC_OFF_CX	68
#define ARC_OFF_CY	76
#define ARC_OFF_CZ	84

#define ARC_OFF_DIR	92

#define ARC_OFF_RAD	94

// -------------------------------------

#define MATRIX_OFF_WI		00
#define MATRIX_OFF_WJ		24
#define MATRIX_OFF_WK		48
#define MATRIX_OFF_WT		72
#define MATRIX_OFF_DI		96
#define MATRIX_OFF_DJ		112
#define MATRIX_OFF_DK		128
#define MATRIX_OFF_DT		144
#define MATRIX_OFF_TL		160
#define MATRIX_OFF_FLAG		162
#define MATRIX_OFF_NAME		164
#define MATRIX_OFF_FREE		173
#define MATRIX_OFF_RADIUS	174

#define MATRIX_OFF_X			0
#define MATRIX_OFF_Y			8
#define MATRIX_OFF_Z			16

// -------------------------------------

#define SUBDEF_OFF_NAME			0
#define SUBDEF_OFF_COUNT		10
#define SUBDEF_OFF_DUMMY0		12
#define SUBDEF_OFF_DUMMY1		13
#define SUBDEF_OFF_DUMMY2		14
#define SUBDEF_OFF_DUMMY3		15

// -------------------------------------

#define SUBCALL_OFF_END			20
#define SUBCALL_OFF_START		44

#define SUBCALL_OFF_STTAN		68
#define SUBCALL_OFF_ENTAN		76

#define SUBCALL_OFF_HANDLE		84

#define SUBCALL_OFF_DELTA		108

#define SUBCALL_OFF_ANGLE		132
#define SUBCALL_OFF_SCALE		140

#define SUBCALL_OFF_COPY		148
#define SUBCALL_OFF_FLAGS		150
#define SUBCALL_OFF_BLANK		152
#define SUBCALL_OFF_XCNT		154
#define SUBCALL_OFF_YCNT		156

#define SUBCALL_OFF_SUBDIR		158

// ------------------------------------------------
// User structure
//
//		00. *			Common
//		20. Point	End
//		44. char[66] Text			65 text + null
//		110.Float	...			// = Unknown
//		114.Float	...			// = Unknown
//		118.Word		...			// = 0

#define USER_OFF_X		20
#define USER_OFF_Y		28
#define USER_OFF_Z		36

#define USER_OFF_TEXT	44		// 66 chars
#define USER_OFF_DUMMY0	110
#define USER_OFF_DUMMY1	114
#define USER_OFF_DUMMY2	118

// -------------------------------------
// Text structure, full of gaps & variable lengths
//
//		00. *			Common
//		20. Point	End
//		44. Word		...
//		46. Word		Length		# chars in buffer incl Null
//		48. Word		iCnt			# integers = 3
//		50. Word		dCnt			# doubles = 5
//		52. Dword	...
//		56. Dword	...
//		60. Dword	...
//		64. char[]	text			size Length
//		(n = 64+Length)			...just after the text...
//		n+0	Word	...			= 0
//		n+2	Word	Length		# chars in buf incl Null
//		n+4	Word	...			= 1
//		n+6	Double Height		Height of characters
//		n+14	Double Angle		Angle of inclination
//		n+22	Double				= 0.2
//		n+30	Double				= 1.0
//		n+38	Double				= 0.0

#define TEXT_OFF_X		20
#define TEXT_OFF_Y		28
#define TEXT_OFF_Z		36

#define TEXT_OFF_DUMMY0	44		// Word = 1
#define TEXT_OFF_LEN		46		// Length includes NULL
#define TEXT_OFF_INT		48		// = 3
#define TEXT_OFF_DBL		50		// = 5
#define TEXT_OFF_DUMMY1	52		// DWord = 0
#define TEXT_OFF_DUMMY2 56		// DWord = 0
#define TEXT_OFF_DUMMY3	60		// DWord = 0

#define TEXT_OFF_TEXT	64

// Subsequent values start after text... + Length

#define TEXT_OFF_DUMMY4	(TEXT_OFF_TEXT+0)		// Word = 0
#define TEXT_OFF_LEN2	(TEXT_OFF_TEXT+2)
#define TEXT_OFF_DUMMY5	(TEXT_OFF_TEXT+4)		// Word = 1

#define TEXT_OFF_HEIGHT	(TEXT_OFF_TEXT+6)
#define TEXT_OFF_ANGLE	(TEXT_OFF_TEXT+14)
#define TEXT_OFF_DUMMY6	(TEXT_OFF_TEXT+22)	// Double = 0.2
#define TEXT_OFF_DUMMY7	(TEXT_OFF_TEXT+30)	// Double = 1.0
#define TEXT_OFF_DUMMY8	(TEXT_OFF_TEXT+38)	// Double = 0.0

#endif