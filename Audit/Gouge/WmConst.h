
#ifndef _WMCONST_H
#define _WMCONST_H

enum EIntersectionType
{
	LeftExitSame       = -8,
	LeftEntryOpposite  = -6,
	LeftExitOpposite   = -3,
	LeftEntrySame      = -2,
	Disengage          = -1,
	Graze              =  0,  // effectually Coincident
	Engage             =  1,
	RightExitSame      =  3,
	RightEntryOpposite =  4,
	RightExitOpposite  =  5,
	RightEntrySame     =  7,
	Undefined          =  8
};

#endif
