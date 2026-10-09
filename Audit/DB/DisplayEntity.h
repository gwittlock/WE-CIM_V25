#if !defined(_DISPLAYENTITY_H)
#define _DISPLAYENTITY_H

// ==================================================================
//		Display Entity
//
//	A single display "thing" -- which links back to a database
//	entity (by ID), and contains the drawing instructions for 
//	that entity.
//
// ==================================================================

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// ==================================================================

#include "Type.h"
#include "ColorConst.h"
#include "Return.h"

#include "3dCoord.h"
#include "3dVec.h"
#include "3x4Matrix.h"

#include "ViewXform.h"
#include "DbEntity.h"


// ==================================================================

enum eDisplayCmd
{
	// Drawing attributes
	DCMD_COLOR,
	DCMD_STYLE,
	DCMD_META,

	// 2d line-drawing commands
	DCMD_MOVETO,		// change current position (also used in 3d)
	DCMD_LINETO,		// draw line to new position
	DCMD_ARCCTR,		// Set the center for drawing an arc...
	DCMD_ARCCCTO,		// Draw a CC arc...
	DCMD_ARCCWTO,		// Draw a CW arc...

/*
	DCMD_SYSMOVETO,		// System: change current position (also used in 3d)
	DCMD_SYSLINETO,		// System: draw line to new position
	DCMD_SYSARCCTR,		// Set the center for drawing an arc...
	DCMD_SYSARCCCTO,	// Draw a CC arc...
	DCMD_SYSARCCWTO,	// Draw a CW arc...

	DCMD_TOOLMOVETO,	// Tool: change current position
	DCMD_TOOLLINETO,	// Tool: draw line to new position
	DCMD_TOOLARCCTR,	// Set the center for drawing an arc...
	DCMD_TOOLARCCCTO,	// Draw a CC arc...
	DCMD_TOOLARCCWTO,	// Draw a CW arc...
*/

	DCMD_TOOLHITCTR,	// Tool:  Center of tool, for solid view
	DCMD_TOOLMARK,		// stand-in marker, when tool not available

	// Text
	DCMD_TEXTANG,		// Set angle for next text; NOT PERSISTENT; resets to zero
	DCMD_TEXTPOS,		// Set positioning for next text; NOT PERSISTENT; resets to default
	DCMD_TEXTSIZE,		// Set the font size; NOT PERSISTANT; resets to default
	DCMD_TEXT,			// Print something...
	DCMD_HOTTEXT,		// Print something, but only when hot!

/*
	// 3d solid commands
	//	(tool construction)
	DCMD_OPENBODY,		// open a tooling body
	DCMD_CLOSEBODY,		// close the tooling body... ready for use
	DCMD_UPVEC,			// Which way is up?
	DCMD_UNION,			// Set union combination mode (for primitive creation)
	DCMD_SPHERE,		// Sphere at position; radius X
	DCMD_CYLINDER,		// Cylinder at position; radius X, length Y
	DCMD_CONE,			// Cone tip at position; end radius X, length Y
	DCMD_TORUS,			// Cone tip at position; outside radius X, minor radius Y

	DCMD_OPENEDGE,		// open a tooling edge-list
	DCMD_CLOSEEDGE,		// close the tooling edge and make it a body
	DCMD_EDGEAT,		// set an edge start point
	DCMD_EDGECTR,		// set an edge center point
	DCMD_EDGETO,		// line edge to
	DCMD_EDGECCTO,		// arc edge counter-clockwise to
	DCMD_EDGECWTO,		// arc edge clockwise to

	//	(routing)
	DCMD_ROUTEAT,		// Cut tool body from stock at position
	DCMD_ROUTETO,		// Route a line with the current tool body
	DCMD_ROUTECTR,		// Set the center for routing an arc...
	DCMD_ROUTECCTO,		// Route a CC arc...
	DCMD_ROUTECWTO,		// Route a CW arc...
*/

	// Selection marking
	DCMD_DOT,
	DCMD_DOTMARK,
	DCMD_SYSDOT,
	DCMD_START,
	DCMD_END,

	DCMD_TARGET
};

enum eDisplayStyle
{
	DSTYLE_SOLID,
	DSTYLE_DASH,
	DSTYLE_SELECT,
	DSTYLE_CENTER,
	DSTYLE_TOOL
};

enum eDisplayTextPos
{
	TEXTPOS_DEFAULT,	// Origin in default top-left position
	TEXTPOS_BTMCTR,		// Text below and centered wrt the origin
	TEXTPOS_TOPCTR		// Text above and centered wrt the origin
};

enum eDisplayMeta
{
	META_NONE,			// Plain graphics
	META_SYS,			// System marks
	META_TOOL,			// Tool graphics
	META_ZONE,			// Zone graphis
	META_ENDS,			// End-point marks
	META_FILL
};

// ==================================================================

class dllExport CDisplayEntity
{
public:

	CDisplayEntity();

	CDisplayEntity( const CDbEntity* in_entity );

	virtual ~CDisplayEntity();

	// Introduced in V19 so as to minimize memory thrashing
	// (ie. by reducing calls to the ctor and dtor).
	void Flush()
		{
			m_command_list.RemoveAll();
			m_world_list.RemoveAll();
		}

	// Use with CDisplayEntity().
	void Init( const CDbEntity* entity );

	const CDbEntity* Entity( void ) const
		{ return m_entity; }

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	tCmdArray* CommandList()
		{ return &m_command_list; }

	int CommandAppend( eDisplayCmd cmd, DWORD param )
		{ return ( m_command_list.Add( Command( cmd, param ) ) ); }

	int CommandAppend( eDisplayCmd cmd, float param )
		{ return ( m_command_list.Add( Command( cmd, param ) ) ); }

	int CommandAppend( const CString& text, int chunk )
		{ return ( m_command_list.Add( Command( text, chunk ) ) ); }

	int countCommand() const
		{ return m_command_list.GetSize(); }

	eDisplayCmd Command( int indx ) const
		{ return extract_command( m_command_list.GetAt( indx ) ); }

	DWORD Param( int indx ) const
		{ return extract_parameter( m_command_list.GetAt( indx ) ); }

	float Float( int indx ) const
		{ return extract_float( m_command_list.GetAt( indx ) ); }

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	tPnt3Array* WorldList()
		{ return &m_world_list; }

	int CoordAppend( const C3dCoord& pt )
		{ return ( m_world_list.Add( pt ) ); }

	int countWorldCoord() const
		{ return m_world_list.GetSize(); }

	const C3dCoord WorldCoord( int indx ) const
		{ return m_world_list.GetAt( indx ); }

	ID ID( void ) const;
	double PntDistance( const C2dCoord& in_pnt, double in_max_dist, bool* io_dot, C3dCoord* io_snap, bool* io_end ) const;
	bool Intersection( CDisplayEntity* ent_2, C3dCoord* int_pnt );

	int decode_text(int cmd_idx, CString* text) const;

public:

	//
	// 2d description commands... called by DbEntity to describe itself
	//
	static __int64	Command( eDisplayCmd cmd, DWORD param )
		{ return ( (((__int64)param)<<32) | (DWORD)cmd); }

	static __int64	Command( eDisplayCmd cmd, float param )
		{ return ( (((__int64)(*((DWORD*)&param)))<<32) | (DWORD)cmd); }

	static int CommandTextNum( const CString& text )
		{
			int len = text.GetLength();
			int chunk = (len + 2 + 7) >> 3;

			return chunk;
		};

	static __int64 Command( const CString& text, int chunk )
		{
			int len = text.GetLength();

			__int64 cmd = 0;
			if (chunk==0)
			{
				cmd = ((__int64)len)<<48;
				for (int idx=0; idx<6; idx++)
				{
					if (idx >= len)
						break;

					cmd |= ((__int64)text[idx]) << (40 - (idx*8));
				}
			}
			else
			{
				for (int cnt=0; cnt<8; cnt++)
				{
					int idx = 6 + ((chunk-1) * 8) + cnt;

					if (idx >= len)
						break;

					cmd |= ((__int64)text[idx]) << (56 - (cnt*8));
				}
			}

			return cmd;
		};

private:

	eDisplayCmd		extract_command( __int64 in_code ) const					{ return (eDisplayCmd)(in_code & 0xffffffff); }
	DWORD			extract_parameter( __int64 in_code ) const				{ return (DWORD)(in_code >> 32); }
	float			extract_float( __int64 in_code ) const						{ DWORD val = (DWORD)(in_code >> 32); return *((float*)&val); }

private:

	// Disabled.
	CDisplayEntity( const CDisplayEntity& );
	const CDisplayEntity& operator = ( const CDisplayEntity& );
	int operator == ( const CDisplayEntity& ) const;
	int operator != ( const CDisplayEntity& ) const;

private:

	tCmdArray	m_command_list;
	tPnt3Array	m_world_list;

	const CDbEntity*	m_entity;	// Reference to entity
};

#endif

