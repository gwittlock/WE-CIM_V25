
#ifndef _DBCONSTS_H
#define _DBCONSTS_H

// NOTE: DBSEQUENCE must be the last real entity type.
// This allows slightly faster file previewing.
// See also CMM2::ReadFile0_001()
#if BEFORE_V16
	enum EDbEntityType
	{
		DBLAYER			=  0,
		DBWORKPLANE		=  1,
		DBTOOL			=  2,
		DBPOINT			=  3,
		DBLINE			=  4,
		DBARC			=  5,
		DBHOLE			=  6,
		DBPROFILE		=  7,
		DBCOMMAND		=  8,  // Should be immediately after DBHOLE but cause file I/O headache.
		DBFEATURE		=  9,
		DBSEQUENCE		= 10,
		DBTERMINAL		= 12  // Terminal marker for looping constructs.
	};
#else
	enum EDbEntityType
	{
		DBLAYER			=  0,
		DBWORKPLANE		=  1,
		DBTOOL			=  2,
		DBPOINT			=  3,
		DBLINE			=  4,
		DBARC			=  5,
		DBHOLE			=  6,
		DBPROFILE		=  7,
		DBCOMMAND		=  8,  // Should be immediately after DBHOLE but cause file I/O headache.
		DBFEATURE		=  9,
		DBPATTERN		= 10,
		DBSEQUENCE		= 11,
		DBTERMINAL		= 12  // Terminal marker for looping constructs.
	};
#endif

typedef DWORD FLAGS;  // Entity display and database state.

#define DBREFERENCE		0x00000001  // Is reference counting turned on?
#define DBCREATED		0x00000002  // Was this entity just created?
#define DBMODIFIED		0x00000004  // Has this entity been modified?
#define DBDELETED		0x00000008  // Has this entity been marked as deleted?
#define DBSYSTEM		0x00000010  // Is this a system-created entity (as opposed to user-created)?
#define DBHIDDEN		0x00000020  // Is this entity hidden?
#define DBSELECTED		0x00000040  // Is this entity selected?
#define DBSELECTABLE	0x00000080  // Is this entity selectable?
//#define DBTAGGED		0x00000100  // Tag marker, for various processing
#define DBSHOWPATH		0x00000200	// Causes change in entity display when Code Viewer is open.
#define DBFILLED		0x00000400	// Causes change in entity display when Code Viewer is open.
#define DBSNAP			0x00000800	// Controls snappable behavior in gview
#define DBHOTDOT		0x00001000	// Controls hot-dot behavior in gview
#endif
