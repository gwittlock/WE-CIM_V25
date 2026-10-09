
#ifndef _APPCONST_H
#define _APPCONST_H

// Context constants

enum EContextConst
{
	VIEW_IDLE,
	VIEW_WINDOW,
	FLUSH_CONTEXT,
	RESTORE_CONTEXT,
	SELECT_ENTITY,
	CREATE_WORKPLANE,
	CREATE_POINT,
	CREATE_LINE,
	CREATE_ARC,
	CREATE_HOLE,
	CREATE_PROFILE
};

enum EEventConst
{
	SET_CONTEXT         = (WM_USER + 1),
	ENTITY_SELECTED     = (WM_USER + 2),
	OFFSET_CURVE        = (WM_USER + 3),
	VIEW_POINT_SELECTED = (WM_USER + 4)
};

#define DROP_CONST	1
#define STOP_CONST	2

#endif
