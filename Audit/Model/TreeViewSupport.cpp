
#include "stdafx.h"
#include "cmn_resource.h"

#include "DbEntity.h"
#include "DbFeature.h"
#include "DbSequence.h"
#include "DbIterator.h"
#include "Model.h"
#include "TreeViewSupport.h"


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// NOTE: This stuff was basically cloned from CSelector, and is probably
// not what we want in the long term because it does not allow the user
// to impose persistant ordering on entities in the tree view.
//
CTreeViewSupport::CTreeViewSupport()
	: m_model( NULL ),
	  m_list()
{
#ifdef LAYER
	m_isAllowed[ DBLAYER ]    = FALSE;
#endif
	m_isAllowed[ DBWORKPLANE ]= FALSE;
	m_isAllowed[ DBTOOL ]     = FALSE;
	m_isAllowed[ DBPOINT ]    = TRUE;
	m_isAllowed[ DBLINE ]     = TRUE;
	m_isAllowed[ DBARC ]      = TRUE;
	m_isAllowed[ DBHOLE ]     = TRUE;
	m_isAllowed[ DBPROFILE ]  = TRUE;
	m_isAllowed[ DBCOMMAND ]  = TRUE;
	m_isAllowed[ DBFEATURE ]  = TRUE;
	m_isAllowed[ DBSEQUENCE ] = FALSE;
	m_isAllowed[ DBPATTERN ]  = TRUE; // For Now...
}

CTreeViewSupport::~CTreeViewSupport()
{
	// ~m_list defaults to BenignFlush()
}

void
CTreeViewSupport::Init( CModel& model )
{
	m_model = &model;
}

int
CTreeViewSupport::Count() const
{
	return m_list.Count();
}

CDbEntity*
CTreeViewSupport::operator [] ( int indx ) const
{
	return m_list[indx];
}

void
CTreeViewSupport::Rebuild()
{
	CDbIterator iter;

	CDbEntity::NewAction();

	m_list.BenignFlush();

	// precursor, drop the patterns in
	if (m_isAllowed[ DBPATTERN ])
	{
		iter.Init( m_model->Db(), DBPATTERN );
		while (1)
		{
			CDbEntity* dbEntity = iter();
			if (dbEntity == NULL)
				break;
			iter.Next();

			if (dbEntity->Type() != DBPATTERN)
				break;

			if ( !dbEntity->DidAction() )
				Select( dbEntity, TRUE );
		}
	}

	iter.Init( m_model->Db(), DBFEATURE );
	while (1)
	{
		CDbFeature*	dbFeature = dynamic_cast<CDbFeature*>( iter() );
		if (dbFeature == NULL)
			break;

		// Be careful to ignore entities that are already selected.
		if ( !dbFeature->DidAction() && dbFeature->IsWorkZone() )
			Select( dbFeature, TRUE );

		iter.Next();
	}

	for (int tndx = (DBTERMINAL-1); tndx >= DBWORKPLANE; --tndx)
	{
		EDbEntityType type = (EDbEntityType) tndx;

		// This optimization allows us to ignore whole sections.
		if ( !m_isAllowed[ type ] )
			continue;

		iter.Init( m_model->Db(), type );
		while (1)
		{
			CDbEntity* dbEntity = iter();

			if (dbEntity == NULL)
				break;

			if (dbEntity->Type() != type)
				break;

			// Be careful to ignore entities that are already selected.
			if ( !dbEntity->DidAction() )
				Select( dbEntity, TRUE );

			iter.Next();
		}
	}
}

// NOTE: This method is recursive.
// If a 'container' type entity is selected,
// so are its contained entities.
CReturn
CTreeViewSupport::Select( CDbEntity* dbEntity, bool checkAllowable )
{
	CReturn status;

	if ( dbEntity->IsSystem() )
		return status;  // Can't select this kind of entity.

	if ( dbEntity->DidAction() )
		return status;  // Nothing to do, already selected.

	if (checkAllowable && !IsAllowed( dbEntity ))
		return status;
	
	CDbEntity* owner = dbEntity->Owner();

	if (owner != NULL && !owner->DidAction() && IsAllowed( owner ))
		status = Select( owner, FALSE );
	else
		status = Add( dbEntity );

	return status;
}

bool
CTreeViewSupport::IsAllowed( const CDbEntity* dbEntity )
{
	if ( dbEntity->IsSystem() )
		return FALSE;

	EDbEntityType type = dbEntity->Type();

	CDbEntity* owner = dbEntity->Owner();

	if (owner != NULL)
	{
		// Higher order entities take precedence over
		// lower order entities.  If a high order entity
		// is selected, all of the entities that it owns
		// will also be selected.

		EDbEntityType ownerType = owner->Type();
		if ( m_isAllowed[ ownerType ] )
			type = ownerType;
	}

	return m_isAllowed[ type ];
}

CReturn
CTreeViewSupport::Add( CDbEntity* dbEntity )
{
	CReturn status;

	dbEntity->DoAction();
	m_list.ConditionalAppend( dbEntity );

	CDbEntityList entities;
	dbEntity->Subordinates( &entities );

	int count = entities.Count();

	for (int indx = 0; indx < count; ++indx)
	{
		CDbEntity* dbEntity = entities[indx];

		if (dbEntity != NULL)  // TODO: How does this happen?
			status = Select( dbEntity, FALSE );

		if ( !status.IsOk() )
			break;
	}

	return status;
}

