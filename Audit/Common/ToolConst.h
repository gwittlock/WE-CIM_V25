
#ifndef _TOOLCONST_H
#define _TOOLCONST_H

// The upper bound on the number of points that
// can be used to describe the shape of a tool.
const int MAX_TOOL_PNT = 1024;

// 'Tool type ids' as defined in the machining database.
#if (_CI || _NST)

	// For CI Wood (must match constants in modDbConsts.bas)
	enum eToolType
	{
		TTYPE_OPEN			= -1,
		TTYPE_NONE			= 0,
		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
		TTYPE_ROUTER_BIT	= 1,
		TTYPE_CORE_BOX		= 2,
		TTYPE_ROUND_OVER	= 3,
		TTYPE_POINT_CUTTING_ROUND_OVER	= 4,
		TTYPE_OGEE	= 5,
		TTYPE_RAISED_PANEL	= 6,
		TTYPE_V_GROOVE		= 7,
		TTYPE_DRILL			= 8,
		TTYPE_SAW			= 9,
		TTYPE_CUSTOM		= 10,
		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
		// TTYPE_ROUTER_BIT	= 1,
		TTYPE_DISC_SAW		= 2,
		TTYPE_BRAD_POINT	= 3,
		TTYPE_LANCE_BIT		= 4,
		TTYPE_COUNTER_SINK	= 5,
		TTYPE_AGGREGATE		= 6,
		TTYPE_TAP			= 7,
		// TTYPE_DRILL			= 8,
		TTYPE_BURNER		= 9,
		TTYPE_SCRIBE		= 10,
		TTYPE_POWDER_MARK	= 11,
		TTYPE_CENTER_PUNCH	= 12,
		TTYPE_LASER			= 13,
		TTYPE_WATERJET		= 14,
		TTYPE_ROUND			= 15,
		TTYPE_SQUARE		= 16,
		TTYPE_RECTANGLE		= 17,
		TTYPE_OBROUND		= 18,
		TTYPE_DIAMOND		= 19,
		TTYPE_CORNER_RADIUS	= 20,
		TTYPE_SINGLE_D		= 21,
		TTYPE_DOUBLE_D		= 22,
		TTYPE_TRAPEZOID		= 23,
		TTYPE_KEYHOLE		= 24,
		TTYPE_FORMING		= 25,
		TTYPE_MARKING		= 26,
		TTYPE_HEXAGON		= 27,
		TTYPE_END_MILL		= 28
		// TTYPE_CUSTOM		= 29
	};

#else

	// For Weng Fab.
	enum eToolType
	{
		TTYPE_OPEN			= -1,
		TTYPE_NONE			= 0,
		TTYPE_ROUTER_BIT	= 1,
		TTYPE_DISC_SAW		= 2,
		TTYPE_BRAD_POINT	= 3,
		TTYPE_LANCE_BIT		= 4,
		TTYPE_COUNTER_SINK	= 5,
		TTYPE_AGGREGATE		= 6,
		TTYPE_TAP			= 7,
		TTYPE_DRILL			= 8,
		TTYPE_BURNER		= 9,
		TTYPE_SCRIBE		= 10,
		TTYPE_POWDER_MARK	= 11,
		TTYPE_CENTER_PUNCH	= 12,
		TTYPE_LASER			= 13,
		TTYPE_WATERJET		= 14,
		TTYPE_ROUND			= 15,
		TTYPE_SQUARE		= 16,
		TTYPE_RECTANGLE		= 17,
		TTYPE_OBROUND		= 18,
		TTYPE_DIAMOND		= 19,
		TTYPE_CORNER_RADIUS	= 20,
		TTYPE_SINGLE_D		= 21,
		TTYPE_DOUBLE_D		= 22,
		TTYPE_TRAPEZOID		= 23,
		TTYPE_KEYHOLE		= 24,
		TTYPE_FORMING		= 25,
		TTYPE_MARKING		= 26,
		TTYPE_HEXAGON		= 27,
		TTYPE_END_MILL		= 28,
		TTYPE_CUSTOM		= 29
	};

#endif

// Given two tools, min( syma, symb ) gives the most restrictive symmetry
enum eToolSymmetry
{
	TSYM_NONE,		// Tool MAY NOT BE ROTATED in use; no symmetry
	TSYM_180,		// Tool may only be rotated by 180`
	TSYM_90,		// Tool has 90` symmetry
	TSYM_1			// Heck, this tool's a whore; it'll do anything
};

// NOTE: This used to be defined in .\NestSupport\RepoZone.h, but
// it needs to be more 'global' in its definition.  And though
// this may not be the best place to define it, it is a safe place.
enum eHoldType
{
	HOLD_2CIRCLE	= 0,
	HOLD_RECTANGLE	= 1
};


enum eMachineType
{
	PUNCH        = 1,
	PUNCH_PLASMA = 2,
	PUNCH_LASER  = 4
};

#endif
