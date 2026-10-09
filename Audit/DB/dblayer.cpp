
#include "stdafx.h"
#include "StringConst.h"
#include "EntityDb.h"
#include "DbLayer.h"
#include "DbEntityVisitor.h"



//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//   -- OBSOLETE -- OBSOLETE -- OBSOLETE -- OBSOLETE -- OBSOLETE --
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

CDbLayer::CDbLayer( CEntityDb* db )
	: CDbEntity( db )
{
	// Copy the default attributes from the database.
	// See also comment in CDbEntity::CDbEntity()
	int color = CDbEntity::Db()->Default().getColor( RGB(255,0,0) );
	ColorSet( color );

	CDbEntity::CreateFlag( false );
}

CDbLayer::CDbLayer( const CDbLayer& dbLayer )
	: CDbEntity( dbLayer )
{
	CDbEntity::CreateFlag( false );
}

CDbLayer::~CDbLayer()
{
	if ( CDbEntity::IsReferencing() )
		CDbEntity::Remove( (*this) );
}

void
CDbLayer::RefsTo( CDbEntityList* list ) const
{
	// Do nothing.  A layer does not reference any entities.
}

void
CDbLayer::Subordinates( CDbEntityList* list ) const
{
	// Do nothing.  A layer does not have any subordinate entities.
}

bool
CDbLayer::HasRefTo( const CDbEntity* refdEntity ) const
{
	// A layer never references another entity.
	return FALSE;
}

void
CDbLayer::Delete()
{
	if ( CDbEntity::IsDeleted() )
		return;

	CDbEntity::DeleteFlag( true );
	CDbEntity::Record();

	CDbEntity::RemoveRefs();
}

void
CDbLayer::Accept( CDbEntityVisitor* visitor )
{
	visitor->Visit( this );
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void
CDbLayer::RemoveRef( CDbEntity* dbEntity )
{
	// Nothing to do.  A layer never references another entity.
	return;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

const CDbLayer&
CDbLayer::operator = ( const CDbLayer& dbLayer )
{
	CDbEntity::CommonCopy( dbLayer );

	return (*this);
}

CReturn
CDbLayer::Clone( CDbEntity** dbEntity ) const
{
	CReturn status;

	CDbLayer* dbLayer = new CDbLayer( (*this) );

	(*dbEntity) = dbLayer;

	return status;
}

CReturn
CDbLayer::ContentsSwap( CDbEntity* dbEntity )
{
	CReturn status;

	CDbLayer* dbLayer = dynamic_cast<CDbLayer*>( dbEntity );

	ASSERT( (dbLayer != NULL) );

	CDbLayer tmp( (*this) );
	(*this) = (*dbLayer);
	(*dbLayer) = tmp;

	// Suppress reference count modifications.
	tmp.ReferenceFlagClear();

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
CReturn 
CDbLayer::CopyTo( 
	CEntityDb*		io_dest,
	tDbEntityMap*	io_refmap, 
	CDbEntity**		dbEntity )
{
	CReturn status;

	//
	// Generic Copy... sets up the database and maps...
	//
	status += CDbEntity::CopyTo( io_dest, io_refmap, dbEntity );
	CDbLayer* dbLayer = (CDbLayer*)(*dbEntity);

	//
	// References and specific data ...
	//
	dbLayer->Name( Name() );

	return status;

}


