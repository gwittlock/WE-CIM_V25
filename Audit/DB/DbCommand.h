
#ifndef _DBCOMMAND_H
#define _DBCOMMAND_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "DbEntity.h"
#include "3dCoord.h"

class CEntityDb;
class CGeoPoint;

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class CEntityDb;



class dllExport CDbCommand : public CDbEntity
{
	friend class CEntityDb;

public:

	// Poor man's RTTI.
	virtual EDbEntityType Type() const   { return DBCOMMAND; }

	// Establish the entity
	void Init( CDbTool* tool, CDbWorkplane *workplane, const C3dCoord&pt, const CString& text );
	void Init( CDbTool* tool, CDbWorkplane *workplane, double x, double y, double z, const CString& text );

	// Access its data
	virtual C3dBox Box( ID workplaneId = ~0 ) const;

	// A convenience method that currently works only for instances.
	C3dBox RealBox( ID workplaneID = ~0 ) const;

	CGeoPoint	Point() const;

	void		Text( const CString& text );
	CString		Text() const			{ return m_text; }
	CString		CommandString() const	{ return m_cmdstr; }

	C3dCoord Coord( ID workplaneId = ~0 ) const;

	// 2009.09.16 (PE) -- Kinda icky. Introduced an override method in
	// support of Transform/Bump. This solution allows the handle point
	// to be drawn in the correct place relative to the pattern (as the
	// instance is being dragged).
	C3dCoord Coord( const C3dCoord& pt );

	// Describe it for display
	virtual bool canDescribe2d( void ) const				{ return TRUE; }
	virtual void Describe2d( int regen, C3dCoord* io_tooltip, double in_tolerance ) const;

	// Transformation
	virtual void Transform( const C3x4Matrix& in_xform );

	// Obtain the list of entities that are referenced by this entity.
	// The entities are appended to the end of the given list.
	virtual void RefsTo( CDbEntityList* list ) const;

	// Returns the same list as RefsTo().
	virtual void Subordinates( CDbEntityList* list ) const;

	// Determine whether this entity references the given entity.
	virtual bool HasRefTo( const CDbEntity* refdEntity ) const;

	virtual void Delete();

	virtual void Accept( CDbEntityVisitor* visitor );

	// Convenience methods.

	bool IsInsert() const;
	bool IsClamp() const;
	bool IsInstance() const;
	bool IsTooledText() const;
	bool IsA( CString cmd ) const;

	CString InstanceLabel() const;

	DWORD TargetColor( DWORD color );

protected:

private:  // Methods

	CDbCommand( CEntityDb* db );

	CDbCommand( const CDbCommand& dbCommand );

	const CDbCommand& operator = ( const CDbCommand& dbCommand );

	virtual ~CDbCommand();

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Methods used by CEntityDb.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	virtual void RemoveRef( CDbEntity* dbEntity );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Methods used by indirectly by CUndo.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	virtual CReturn Clone( CDbEntity** dbEntity ) const;

	virtual CReturn ContentsSwap( CDbEntity* dbEntity );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	virtual CReturn CopyTo( CEntityDb* io_dest, tDbEntityMap* io_refmap, CDbEntity** dbEntity );

private:  // Disabled

	CDbCommand();
	int operator == ( const CDbCommand& ) const;
	int operator != ( const CDbCommand& ) const;

private:  // Data

	C3dCoord	m_pt;
	CString		m_text;
	CString		m_cmdstr;

	DWORD		m_target_color;
};

#endif

