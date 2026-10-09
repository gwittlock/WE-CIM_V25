
#ifndef _DBLAYER_H
#define _DBLAYER_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef _DBENTITY_H
#include "DbEntity.h"
#endif

class CEntityDb;
class C3dCoord;


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//   -- OBSOLETE -- OBSOLETE -- OBSOLETE -- OBSOLETE -- OBSOLETE --
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class dllExport CDbLayer : public CDbEntity
{
	friend class CEntityDb;

public:

	// Poor man's RTTI.
	virtual EDbEntityType Type() const   { return DBLAYER; }

	virtual void RefsTo( CDbEntityList* list ) const;

	virtual bool HasRefTo( const CDbEntity* refdEntity ) const;

	virtual void Subordinates( CDbEntityList* list ) const;

	virtual void Delete();

	virtual void Accept( CDbEntityVisitor* visitor );

	virtual CReturn CopyTo( CEntityDb* io_dest, tDbEntityMap* io_refmap, CDbEntity** dbEntity );

protected:

private:  // Methods

	CDbLayer( const CDbLayer& dbLayer );

	CDbLayer( CEntityDb* db );

	const CDbLayer& operator = ( const CDbLayer& dbLayer );

	virtual ~CDbLayer();

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

	CDbLayer();
	int operator == ( const CDbLayer& ) const;
	int operator != ( const CDbLayer& ) const;

private:

};

#endif

