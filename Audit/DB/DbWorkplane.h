
#ifndef _DBWORKPLANE_H
#define _DBWORKPLANE_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef _DBENTITY_H
#include "DbEntity.h"
#endif

#ifndef _3X4MATRIX_H
#include "3x4Matrix.h"
#endif

class CEntityDb;


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class dllExport CDbWorkplane : public CDbEntity
{
	friend class CEntityDb;

public:

	// Poor man's RTTI.
	virtual EDbEntityType Type() const   { return DBWORKPLANE; }

	virtual CDbWorkplane* Workplane() const		{ return NULL; }
	virtual CDbTool* Tool() const				{ return NULL; }

	virtual void Workplane( CDbWorkplane* dbWork )	{ dbWork;  /* compiler fodder, do nothing */ }
	virtual void Tool( CDbTool* dbTool )			{ dbTool;  /* compiler fodder, do nothing */ }

	void Init(	const C3dVec& ivec, 
				const C3dVec& jvec, 
				const C3dVec& kvec, 
				const C3dCoord& origin,
				bool			up );

	const C3x4Matrix& Transform() const;

	const C3x4Matrix& Inverse() const;

	virtual void RefsTo( CDbEntityList* list ) const;

	virtual bool HasRefTo( const CDbEntity* refdEntity ) const;

	// Returns the same list as RefsTo().
	virtual void Subordinates( CDbEntityList* list ) const;

	// NOTE: A workplane can not be deleted as long
	// as it is still referenced by other entities.
	virtual void Delete();

	virtual void Accept( CDbEntityVisitor* visitor );

	virtual bool canDescribe2d( void ) const { return TRUE; }
	virtual void Describe2d( int regen, C3dCoord* io_tooltip, double in_tolerance ) const;

	int		ToolUp( void ) const			{ return m_tool_up; }
	void	ToolUp( int in_up )				{ m_tool_up = ((in_up < 0) ? -1 : 1); }

	virtual CReturn CopyTo( CEntityDb* io_dest, tDbEntityMap* io_refmap, CDbEntity** dbEntity );

protected:

private:  // Methods

	CDbWorkplane( CEntityDb* db );

	CDbWorkplane( const CDbWorkplane& dbWorkplane );

	const CDbWorkplane& operator = ( const CDbWorkplane& dbWorkplane );

	virtual ~CDbWorkplane();

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

private:  // Disabled

	CDbWorkplane();
	int operator == ( const CDbWorkplane& ) const;
	int operator != ( const CDbWorkplane& ) const;

private:  // Data

	C3x4Matrix	m_xform;
	C3x4Matrix	m_inverse;
	int			m_tool_up;	// tool k vector = m_xform.K() * m_tool_up
};

#endif

