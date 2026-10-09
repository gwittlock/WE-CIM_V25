
#ifndef _DBPROFILE_H
#define _DBPROFILE_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "DbEntity.h"
#include "DbCurveList.h"
#include "DbContainer.h"

class CDbCurve;



//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class CEntityDb;



class dllExport CDbProfile : public CDbContainer
{
	friend class CEntityDb;
	friend class CMM2;

public:

	// Poor man's RTTI.
	virtual EDbEntityType Type() const   { return DBPROFILE; }

	virtual CDbWorkplane* Workplane() const;
	virtual CDbTool* Tool() const;

	virtual void Workplane( CDbWorkplane* dbWork )	{ dbWork; /* compiler fodder, do nothing */ }
	virtual void Tool( CDbTool* dbTool )			{ dbTool; /* compiler fodder, do nothing */ }

	virtual int Count() const;

	virtual CDbEntity* GetAt( int indx ) const;
	virtual CDbEntity* operator[]( int indx ) const;

	virtual CDbEntity* ReplaceAt( int indx, CDbEntity* dbEntity );

	CReturn Prepend( CDbCurve* dbCurve );
	CReturn Append( CDbCurve* dbCurve );

	CReturn Append( const CDbCurveList& curves );

	virtual CReturn Prepend( CDbEntity* dbEntity, bool copy = FALSE );
	virtual CReturn Append( CDbEntity* dbEntity, bool copy = FALSE );

	// NOTE: Use these method wisely.  You must be careful
	// processing associated profiles (eg.CDbCurve::Split())
	virtual int InsertBefore( CDbEntity* refCurve, CDbEntity* newCurve );
	virtual int InsertBefore( int indx, CDbEntity* newEntity );

	virtual int InsertAfter( CDbEntity* refCurve, CDbEntity* newCurve );
	virtual int InsertAfter( int indx, CDbEntity* newEntity );

	int InsertBefore( CDbCurve* refCurve, CDbCurve* newCurve );
	int InsertAfter( CDbCurve* refCurve, CDbCurve* newCurve );

	// CDbContainer requirement -- releases ownership of the given entity.
	// Returns true if successful.
	virtual bool Disown( CDbEntity* dbEntity );

	virtual void BenignFlush();
	virtual CReturn DestructiveFlush();

	void Reverse();

	bool IsAssociated() const			{ return m_associated; }
	CReturn Associate( double tol );
	CReturn Disassociate();

	// Determine whether this profile is closed within system tolerance.
	bool IsClosed() const;

	// Determine whether this profile is closed within the given tolerance.
	bool IsClosed( double tol ) const;

	bool CanClose() const;

	// Obtain the list of entities that are referenced by this entity.
	// The entities are appended to the end of the given list.
	virtual void RefsTo( CDbEntityList* list ) const;

	// Determine whether this entity references the given entity.
	virtual bool HasRefTo( const CDbEntity* refdEntity ) const;

	// Returns the same list as RefsTo().
	virtual void Subordinates( CDbEntityList* list ) const;

	// Get the atomic entities.
	virtual void Flatten( CDbEntityList* entities ) const;

	// Find the position of the given curve in the profile (0 indexed).
	int Position( const CDbCurve* curve ) const;
	virtual int Position( const CDbEntity* refEntity ) const;

	virtual void Delete();

	virtual void Accept( CDbEntityVisitor* visitor );

	// Obtain the bounding box containing all profile entities.
	virtual C3dBox Box( ID workplaneId = ~0 ) const;

	virtual bool canDescribe2d( void ) const { return TRUE; }
	virtual void Describe2d( int regen, C3dCoord* io_tooltip, double in_tolerance ) const;

	virtual void Transform( const C3x4Matrix& in_xform );

	bool IsLeadHull() const;

public:

	static bool IsUsableCurve( const CDbEntity& dbEntity );

protected:

private:  // Methods

	CDbProfile( CEntityDb* db );

	CDbProfile( const CDbProfile& dbProfile );

	const CDbProfile& operator = ( const CDbProfile& dbProfile );

	virtual ~CDbProfile();

	CReturn Append( CDbCurve* dbEntity, bool copy );

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

	CDbProfile();
	int operator == ( const CDbProfile& ) const;
	int operator != ( const CDbProfile& ) const;

private:  // Data

	// The curves owned by this profile.
	CDbCurveList m_list;

	// Indicates whether the curves are associated with
	// common end points, introducing 'rubber band' behavior.
	bool m_associated;
};

#endif

