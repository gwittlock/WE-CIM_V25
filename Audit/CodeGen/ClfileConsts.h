
#ifndef _CLFILECONSTS_H
#define _CLFILECONSTS_H

// Reserved Record Types (IDs) recognized by the modeling
// system, the clfile generator and the post processor.
// NOTE: The user can define his own constants outside
// of the given reserved ranges.

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Constants used by the switch statement in Post.main()
// Some of these constants are associated with a template
// that the user defined in his post configuration.
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
const int EV_RAPID					= 0;
const int EV_LINE					= 1;
const int EV_CW_ARC					= 2;
const int EV_CCW_ARC				= 3;

const int EV_FIRST_POINT			= 20;
const int EV_FIRST_DRILL_HOLE		= 21;
const int EV_FIRST_PECK_HOLE		= 22;

const int EV_POINT					= 23;
const int EV_DRILL_HOLE				= 24;
const int EV_PECK_HOLE				= 25;

const int EV_DISENGAGE				= 43;
const int EV_TEXT_COMMAND			= 44;
const int EV_USER_COMMAND			= 45;
const int EV_HOLD_COMMAND			= 46;
const int EV_REPO_COMMAND			= 47;
const int EV_STOP_COMMAND			= 48;
const int EV_DROP_COMMAND			= 49;
const int EV_CLAMP_INFO				= 50;
const int EV_SPEED_RAMP_INFO		= 51;

const int EV_MAIN_BEGIN				= 60;
const int EV_MAIN_END				= 61;
const int EV_START_PROGRAM_BEGIN	= 62;
const int EV_START_PROGRAM_END		= 63;
const int EV_END_PROGRAM_BEGIN		= 64;
const int EV_END_PROGRAM_END		= 65;
const int EV_TOOL_CHANGE_BEGIN		= 66;
const int EV_TOOL_CHANGE_END		= 67;

const int EV_SUBDEF_BEGIN			= 80;
const int EV_SUBDEF_END				= 81;
const int EV_SUBCALL				= 82;

const int EV_WORKZONE_INFO			= 100;

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Constants used to define data items with a clfile
// record.  As the clfile is designed to have three banks
// of registers (int, double, string) these constants are
// divide accordingly.
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

const int DBL_XS                   = 0;
const int DBL_YS                   = 1;
const int DBL_ZS                   = 2;
const int DBL_XE                   = 3;
const int DBL_YE                   = 4;
const int DBL_ZE                   = 5;
const int DBL_XC                   = 6;
const int DBL_YC                   = 7;
const int DBL_ZC                   = 8;
const int DBL_DEPTH                = 9;

#endif
