
#include "stdafx.h"
#include "cmn_resource.h"
#include "MathConst.h"

#include "3dBox.h"
#include "DbEntity.h"
#include "DbTool.h"
#include "DbCurve.h"
#include "DbFeature.h"
#include "DbIterator.h"
#include "GeoLine.h"
#include "Int2d.h"
#include "Model.h"
#include "Selector.h"

const bool FILTERED = TRUE;


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

CSelector::CSelector( CModel& model )
	: m_model( &model ),
	  m_selected(),
	  m_firstSelected( NULL ),
	  m_checkSystemFlag( TRUE ),
	  m_restricted( FALSE )
{
	// By default, everything is selectable.
	//
	// (2006.07.12 PE) -- This lower bound of this loop
	// used to be DBWORKPLANE but BoundsChecker complained
	// elsewhere that m_isAllowed[DBLAYER] was uninitialized.
	// So, we do this even though DBLAYER is obsolete.
	for (int indx = DBLAYER; indx < DBTERMINAL; ++indx)
	{
		m_isAllowed[indx] = TRUE;
	}
}

CSelector::~CSelector()
{
	// BugID: 380 -- Instantiation of CSelector on stack by nesting
	// operations was diddling with selection state of entities.  This
	// was not apparent until Manual Repo became usable.  This was not
	// a problem previously because nesting works in a pristine environment.
	//
	// Now, when a selector is destroyed, the selection state of the
	// selected entities are cleared.  This solution requires a counter-
	// measure, however (see also NestProcess\NestProcess.cpp and search
	// for the string "BugID: 380")

	Clear();
}

bool
CSelector::IsSelected( ID id ) const
{
	int count = m_selected.Count();

	for (int indx = 0; indx < count; ++indx)
	{
		CDbEntity* dbEntity = m_selected[indx];

		if (dbEntity->Id() == id)
			return TRUE;
	}

	return FALSE;
}

void
CSelector::Clear()
{
	int count = m_selected.Count();

	for (int indx = 0; indx < count; ++indx)
	{
		CDbEntity* dbEntity = m_selected.Remove( 0 );

		dbEntity->SelectFlag( false );
	}
}

void
CSelector::All( int state )
{
	for (int type = DBWORKPLANE; type < DBTERMINAL; ++type)
	{
		Filter( (EDbEntityType) type, state );
	}
}

void
CSelector::Filter( EDbEntityType type, int state )
{
	m_isAllowed[type] = ((state == 0) ? FALSE : TRUE);
}

// The given id may come from either the graphics view or the tree view.
// Whereas containers can be directly selected in the tree view, they
// can only be inferred by proxy in the graphics view.
CReturn
CSelector::FilteredSelect( ID id )
{
	CDbEntity* dbEntity;
	m_model->EntityFind( id, &dbEntity );

	if (dbEntity == NULL)
		return CReturn( STATUS_ERROR );

	return ( FilteredSelect( dbEntity, TRUE ) );
}

// The given id may come from either the graphics view or the tree view.
// Whereas containers can be directly selected in the tree view, they
// can only be inferred by proxy in the graphics view.
CReturn
CSelector::FilteredSelect( CDbEntity* dbEntity, bool considerHiddenStatus )
{
	CReturn status;

	if ( !dbEntity->IsSelected() )
	{
		m_firstSelected = NULL;

		// Find the immediate top-level container as required.  Where the tree
		// view is concerned, it is possible the given id is the id of the
		// immediate container (ie. a profile or feature was directly selected).
		while (1)
		{
			if (dbEntity == NULL)
				break;

			if ( IsTarget( (*dbEntity) ) )
				break;

			dbEntity = (NeedOwner( (*dbEntity) ) ? dbEntity->Owner() : NULL);
		}

		if (dbEntity != NULL)
			Add( dbEntity, considerHiddenStatus );
	}

	return status;
}

// Add this entity to the selection set and mark it as selected.  Also,
// recursively add all of this entity's subordinate entities as required.
//
// NOTE: The m_restricted flag was introduced to augment entity selection
// behavior via Java.  In said case, it often desirable to build a selection
// set that contains only profile entities, for instance.
void
CSelector::Add( CDbEntity* dbEntity, bool considerHiddenStatus )
{
	if (m_checkSystemFlag && dbEntity->IsSystem())
		return;  // Can't select this kind of entity.

	// BugID:42 -- A hidden layer can not be restored.
	//   The parameter 'considerHiddenStatus' was introduced to accomodate
	//   different uses of CSelector::Add().  In the default case, where
	//   an end-user is selecting an entity via the ui, we must disallow
	//   selection of hidden entities.  However, when the application is
	//   selecting all entities of a given type (eg. for the Layer dialog)
	//   we need to ignore the 'hidden' status of the entity.
	//
#if V16_ORIGINAL
	if ( considerHiddenStatus && m_model->is_hidden( dbEntity ) )
	{
		// Be careful not to select hidden entities.  Otherwise,
		// unexpected things happen during subsequent operations
		// such as delete and transform.
		//
		// Workplanes are a special case, however.  Workplanes
		// are almost always hidden, yet when reading an mm2 the
		// application finds the available workplanes by executing
		// a 'select all workplanes'.  If workplanes are ignored,
		// the application will failed to load the model, and the
		// 'Startup Wizard' will appear.

		if (dbEntity->Type() != DBWORKPLANE)
			return;
	}
#else
	if ( considerHiddenStatus && m_model->is_hidden( dbEntity ) )
	{
		// Be careful not to select hidden entities.  Otherwise,
		// unexpected things happen during subsequent operations
		// such as delete and transform.
		//
		// Workplanes are a special case, however.  Workplanes
		// are almost always hidden, yet when reading an mm2 the
		// application finds the available workplanes by executing
		// a 'select all workplanes'.  If workplanes are ignored,
		// the application will failed to load the model, and the
		// 'Startup Wizard' will appear.

		if (dbEntity->Type() != DBWORKPLANE)
			return;
	}
	else if (dbEntity->Type() == DBPOINT && dbEntity->IsSystem())
	{
		return;
	}
#endif

	if (m_firstSelected == NULL)
		m_firstSelected = dbEntity;

	if ( (dbEntity->Type() == DBFEATURE)
		&& ((CDbFeature*) dbEntity)->IsWorkZone()
		&& !m_allowWorkzone )
	{
		// Do nothing.
	}
	else
	{
		dbEntity->SelectFlag( true );
		m_selected.ConditionalAppend( dbEntity );
	}

	if ( !m_restricted )
	{
		// Add all subordinate entities.

		CDbEntityList entities;
		dbEntity->Subordinates( &entities );

		int count = entities.Count();
		for (int indx = 0; indx < count; ++indx)
		{
			Add( entities[indx], considerHiddenStatus );
		}
	}
}

CReturn
CSelector::FilteredDeselect( ID id )
{
	CDbEntity* dbEntity;
	m_model->EntityFind( id, &dbEntity );

	if (dbEntity == NULL)
		return CReturn( STATUS_ERROR );

	return ( FilteredDeselect( dbEntity ) );
}

CReturn
CSelector::FilteredDeselect( CDbEntity* dbEntity )
{
	CReturn	status;
	bool	unselect;

	// During V16-beta, it became apparent that you could select
	// everything in a work-zone via the Entity List, but you
	// could not unselect same via the Entity List.
	if (dbEntity->Type() == DBFEATURE && ((CDbFeature*) dbEntity)->IsWorkZone())
		unselect = TRUE;
	else
		unselect = dbEntity->IsSelected();

	if ( unselect )
	{
		// Find the immediate top-level container as required.  Where the tree
		// view is concerned, it is possible the given id is the id of the
		// immediate container (ie. a profile or feature was directly selected).
		while (1)
		{
			if (dbEntity == NULL)
				break;

			if ( IsTarget( (*dbEntity) ) )
				break;

			dbEntity = (NeedOwner( (*dbEntity) ) ? dbEntity->Owner() : NULL);
		}

		if (dbEntity != NULL)
			Remove( dbEntity );
	}

	return status;
}

CReturn
CSelector::Remove( CDbEntity* dbEntity )
{
	CReturn status;
	int count, indx;

#if REQUIRED
	if (m_checkSystemFlag && dbEntity->IsSystem())
		return;  // Can't select this kind of entity.
#endif

	// Remove the given entity
	indx = m_selected.Find( dbEntity );
	if (indx >= 0)
	{
		m_selected.Remove( indx );
		dbEntity->SelectFlag( false );
	}

	// Remove the subordinate entities.
	CDbEntityList entities;
	dbEntity->Subordinates( &entities );

	count = entities.Count();
	for (indx = 0; indx < count; ++indx)
	{
		status = Remove( entities[indx] );
		if ( !status.IsOk() )
			break;
	}

	return status;
}

// In support of Java ...
CReturn
CSelector::Add( ID id )
{
	CDbEntity* dbEntity;
	m_model->EntityFind( id, &dbEntity );

	CReturn status = ((dbEntity == NULL) ? STATUS_ERROR : STATUS_OKAY);

	if ( status.IsOk() )
	{
		if (m_checkSystemFlag && dbEntity->IsSystem())
			return status;  // Can't select this kind of entity.

		if ( dbEntity->IsDeleted() )
			return status;  // Can't select deleted entities (like duh!)

		if ( dbEntity->IsSelected() )
			return status;  // Nothing to do, already selected.

		if (dbEntity->Type() == DBFEATURE && ((CDbFeature*) dbEntity)->IsWorkZone())
		{
			// Do nothing.
		}
		else
		{
			dbEntity->SelectFlag( true );
			m_selected.ConditionalAppend( dbEntity );
		}
	}

	return status;
}

// In support of Java ...
CReturn
CSelector::Remove( ID id )
{
	CReturn status;

	CDbEntity* dbEntity;
	status = m_model->EntityFind( id, &dbEntity );

	if (dbEntity != NULL)
	{
		int indx = m_selected.Find( dbEntity );
		if (indx >= 0)
		{
			m_selected.Remove( indx );
			dbEntity->SelectFlag( false );
		}
	}

	return status;
}

CReturn
CSelector::SelectAllRefsTo( ID id )
{
	CDbEntity* dbEntity;

	CReturn status = m_model->EntityFind( id, &dbEntity );

	if ( status.IsOk() )
	{
		CDbEntityList entities;
		dbEntity->RefdBy( &entities );

		int count = entities.Count();
		for (int indx = 0; indx < count; ++indx)
		{
			status = FilteredSelect( entities[indx], TRUE );
			if ( !status.IsOk() )
				break;
		}
	}

	return status;
}

CReturn
CSelector::DeselectAllRefsTo( ID id )
{
	CDbEntity* dbEntity;

	CReturn status = m_model->EntityFind( id, &dbEntity );

	if ( status.IsOk() )
	{
		CDbEntityList entities;
		dbEntity->RefdBy( &entities );

		int count = entities.Count();
		for (int indx = 0; indx < count; ++indx)
		{
			status = FilteredDeselect( entities[indx] );
			if ( !status.IsOk() )
				break;
		}
	}

	return status;
}

// Prior to V15, box selection of entities was wrongly based upon
// intersection between the entity bounding box and the selection box.
//
// With V15, box selection has been enhanced.  First, trivial rejection
// is performed based on intersection between an entity's bounding box
// and the selection box.  Passing trivial rejection, intersections
// between an entity and the edges of the selection box are found.
// Failing that, whole containment of the entity by the selection box
// is determined.
//
void
CSelector::SelectBox( const C2dBox& box2d, double tol )
{
	CDbIterator iter;
	CGeoLine boxGeo[4];
	bool featState, profState, arcState, lineState;

	// The edges of the selection box are cached as geometric
	// lines (to mimimize calls to the CGeoLine constructor).
	boxGeo[0].StartPt( box2d.Xmin(), box2d.Ymin(), 0. );
	boxGeo[0].EndPt( box2d.Xmin(), box2d.Ymax(), 0. );

	boxGeo[1].StartPt( box2d.Xmin(), box2d.Ymax(), 0. );
	boxGeo[1].EndPt( box2d.Xmax(), box2d.Ymax(), 0. );

	boxGeo[2].StartPt( box2d.Xmax(), box2d.Ymax(), 0. );
	boxGeo[2].EndPt( box2d.Xmax(), box2d.Ymin(), 0. );

	boxGeo[3].StartPt( box2d.Xmax(), box2d.Ymin(), 0. );
	boxGeo[3].EndPt( box2d.Xmin(), box2d.Ymin(), 0. );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Munge the selection filters so the application doesn't have to.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	featState = m_isAllowed[DBFEATURE];
	profState = m_isAllowed[DBPROFILE];
	arcState  = m_isAllowed[DBARC];
	lineState = m_isAllowed[DBLINE];

	if ( m_isAllowed[DBFEATURE] )
	{
		m_isAllowed[DBPROFILE] = TRUE;
		m_isAllowed[DBHOLE] = TRUE;
	}

	if ( m_isAllowed[DBPROFILE] )
	{
		m_isAllowed[DBARC] = TRUE;
		m_isAllowed[DBLINE] = TRUE;
	}

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Do the entity selection.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	// NOTE: We can ignore direct consideration for container entities
	// like DBPROFILE, DBFEATURE & DBSEQUENCE because they are represented
	// by their contained geometry entities.
	//
	for (int tndx = DBWORKPLANE; tndx <= DBCOMMAND; ++tndx)
	{
		EDbEntityType type = (EDbEntityType) tndx;

		// This optimization allows us to ignore whole buckets of entities.
		if ( !m_isAllowed[ type ] )
			continue;

		if (type == DBPROFILE)
			continue;  // See NOTE: above

		iter.Init( m_model->Db(), type );
		while (1)
		{
			CDbEntity* dbEntity = iter();

			if (dbEntity == NULL)
				break;

			if (dbEntity->Type() != type)  // TODO: Is this statement necessary?
				break;

			// Be careful to ignore entities that are already selected.
			if ( !dbEntity->IsSelected() )
			{
				if ( BoxSelection( box2d, boxGeo, dbEntity, tol ) )
				{
					// Find the topmost 'selectable' parent and select the entire thing.
					CDbEntity* owner = TopLevelEntityFind( dbEntity );

					if (owner != NULL)
						Add( owner, TRUE );
				}
			}

			iter.Next();
		}
	}

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Restore the filters.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	m_isAllowed[DBFEATURE] = featState;
	m_isAllowed[DBPROFILE] = profState;
	m_isAllowed[DBARC] = arcState;
	m_isAllowed[DBLINE] = lineState;
}

void
CSelector::WithinBox( 
	const C3dBox& box,	// IN WORLD COORDINATES
	double tol )
{
	CDbIterator iter;

	// The database is traversed from higher order to lower order
	// entities because entity selection is recursive.  That is,
	// a higher order entity can own a lower order entity.  When
	// the higher order entity is selected, all of it subordinate
	// entities are selected as well (based on the selection filter).
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
			if ( !dbEntity->IsSelected() )
			{
				C3dBox entityBox = dbEntity->Box(0);

				if ( box.Contains( entityBox, tol ) )
					FilteredSelect( dbEntity, TRUE );
			}

			iter.Next();
		}
	}
}

void
CSelector::FeatureWithinBox( 
	const C3dBox& box,	// IN WORLD COORDINATES
	double tol )
{
	CDbIterator iter;

	//
	// Kludged in for Nesting... this specifically knows to look for
	//	features; and it travels up the feature heirarchy to the TOP
	//	feature before checking inclusion.
	//
	// The database is traversed from higher order to lower order
	// entities because entity selection is recursive.  That is,
	// a higher order entity can own a lower order entity.  When
	// the higher order entity is selected, all of it subordinate
	// entities are selected as well (based on the selection filter).
	//
	CDbEntity::NewAction();
	
	iter.Init( m_model->Db(), DBFEATURE );
	while (1)
	{
		CDbEntity* dbEntity = iter();

		if (dbEntity == NULL)
			break;

		if (dbEntity->Type() != DBFEATURE)
			break;

		// Be careful to ignore entities that are already selected.
//		while (dbEntity->Owner() && !dbEntity->IsSelected())
//			dbEntity = dbEntity->Owner();
		while (dynamic_cast<CDbFeature*>(dbEntity->Owner()) )
			dbEntity = dbEntity->Owner();

		if (!dbEntity->IsSelected() && !dbEntity->DidAction())
		{
			C3dBox entityBox = dbEntity->Box(0);

			if ( box.Contains( entityBox, tol ) )
				FilteredSelect( dbEntity, TRUE );
			else
				dbEntity->DoAction();
		}

		iter.Next();
	}
}

void
CSelector::SelectAll( bool considerHiddenStatus )
{
	CDbIterator		iter;
	CDbFeature*		dbFeature;
	CDbEntity*		dbEntity;
	EDbEntityType	type;
	int				tndx;
	bool			defered;

	// The database is traversed from higher order to lower order
	// entities because entity selection is recursive.  That is,
	// a higher order entity can own a lower order entity.  When
	// the higher order entity is selected, all of it subordinate
	// entities are selected as well (based on the selection filter).

	for (tndx = (DBTERMINAL-1); tndx >= DBLAYER; --tndx)
	{
		type = (EDbEntityType) tndx;

		// This optimization allows us to ignore whole sections.
		if ( !m_isAllowed[ type ] )
			continue;

		iter.Init( m_model->Db(), type );
		while (1)
		{
			dbEntity = iter();

			if (dbEntity == NULL || dbEntity->Type() != type)
				break;

#ifdef V15_1
			// Be careful to ignore entities that are already selected.
			if ( !dbEntity->IsSelected() )
				FilteredSelect( dbEntity, considerHiddenStatus );
#else
			// 2006.09.19 (PE) - V18 Beta -- Wow, this bug has been here a
			// long time. We noticed that some features in the entity list
			// were not marked as selected after selecting the the '+' button.
			// Bottomline: Where features are concerned, we defer eligibility
			// for selection to the top-level feature (that is not a workzone).
			defered = false;
			if (dbEntity->Type() == DBFEATURE)
			{
				dbFeature = dynamic_cast<CDbFeature*>( dbEntity->Owner() );
				if (dbFeature != NULL)
					defered = !dbFeature->IsWorkZone();
			}

			// BugID: 605 -- Select/All followed by Edit/Delete was
			// deleting all hidden entities.
			//
			// NOTE: With V16, features are no longer associated with
			// a tool/layer.  CanSelect() is now used instead of
			// FilteredSelect() because CanSelect() takes this change
			// into consideration.
			if ( !defered && CanSelect( dbEntity ) )
				Add( dbEntity, considerHiddenStatus );
#endif

			iter.Next();
		}
	}
}

int
CSelector::Filters()
{
	// there's gotta be a better way
	int filters = 0;

#ifdef LAYER
	filters += (m_isAllowed[DBLAYER]	* 0x001);
#endif
	filters += (m_isAllowed[DBWORKPLANE]* 0x002);
	filters += (m_isAllowed[DBTOOL]		* 0x004);
	filters += (m_isAllowed[DBPOINT]	* 0x008);
	filters += (m_isAllowed[DBLINE]		* 0x010);
	filters += (m_isAllowed[DBARC]		* 0x020);
	filters += (m_isAllowed[DBHOLE]		* 0x040);
	filters += (m_isAllowed[DBPROFILE]	* 0x080);
	filters += (m_isAllowed[DBCOMMAND]	* 0x100);
	filters += (m_isAllowed[DBFEATURE]	* 0x200);

	return filters;
}

bool
CSelector::IsTarget( const CDbEntity& dbEntity )
{
	EDbEntityType type = dbEntity.Type();

	return ( m_isAllowed[ type ] );
}

// NOTES:
// 1. layers, workplanes and tools do not have owners
// 2. points through holes (inclusive) are considered atomic
// 3. commands do owners because they are used for graphics text
// 4. v14 and prior, points do not have owners
// 5. holes can only be owned by features
bool
CSelector::NeedOwner( const CDbEntity& dbEntity )
{
	EDbEntityType type = dbEntity.Type();

	if ( IsTarget( dbEntity ) )
		return FALSE;

	if ( type <= DBPOINT   ||
		 type == DBCOMMAND ||
		 type == DBFEATURE )
	{
		return FALSE;
	}

	if (type == DBLINE || type == DBARC)
	{
		type = DBPROFILE;
	}
	else if (type == DBPROFILE || type == DBHOLE)
	{
		type = DBFEATURE;
	}

	while (type < DBTERMINAL)
	{
		if ( m_isAllowed[ type ] )
			return TRUE;

		type = (EDbEntityType) (type + 1);
	}

	return FALSE;
}

bool
CSelector::BoxSelection(
					const C2dBox&		box2d,
					const CGeoLine*		boxGeo,
					const CDbEntity*	dbEntity,
					double				tol )
{
	CInt2d intersector;
	C2dBox entityBox;
	const CDbCurve* dbCurve;
	CGeoCurve* geoCurve;
	EDbEntityType type;
	int indx;

	bool selected = FALSE;

	entityBox = dbEntity->Box();

	if ( box2d.Intersects( entityBox, tol ) )
	{
		type = dbEntity->Type();

		if (type == DBPOINT || type == DBHOLE || type == DBCOMMAND)
		{
			selected = TRUE;
		}
		else if (type == DBLINE || type == DBARC)
		{
			dbCurve = dynamic_cast<const CDbCurve*>( dbEntity );
			geoCurve = dbCurve->Curve();  // TODO: Why not a 'world curve'?

			// Check for intersections between the curve
			// and the edges of the selection box.
			for (indx = 0; indx < 4; ++indx)
			{
				intersector.CrvCrv( (*geoCurve), boxGeo[indx] );
				if (intersector.Count() > 0)
				{
					selected = TRUE;
					break;
				}
			}

			delete geoCurve;

			if ( !selected )
			{
				// Check if the entity is wholly contained by the selection box.
				selected = box2d.Contains( entityBox, tol );
			}
		}
		else
		{
			// Profiles and features can not be selected directly via
			// the UI because geometric entities are there representatives.
		}
	}

	return selected;
}

// Travel up the hierarchy and find the topmost selectable entity.
// This will select and entire part (if it exists) but not a workzone.
// NOTE: Introduced for 'selection by box'.
//
CDbEntity*
CSelector::TopLevelEntityFind( CDbEntity* dbEntity )
{
	CDbEntity* top;
	CDbEntity* owner;
	EDbEntityType type;
	CString name;

	top = dbEntity;
	type = dbEntity->Type();

	while (1)
	{
		owner = top->Owner();
		if (owner == NULL)
			break;

		type = owner->Type();
		if ( !m_isAllowed[type] )
			break;

		if (type == DBFEATURE && ((CDbFeature*) owner)->IsWorkZone())
			break;

		top = owner;
	}

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Filter out improper selections.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	if ( m_isAllowed[DBFEATURE] )
	{
		if (top->Type() != DBFEATURE)
			top = NULL;
	}
	else if ( m_isAllowed[DBPROFILE] )
	{
		if (top->Type() != DBPROFILE) // || top->Owner() != NULL)
			top = NULL;
	}

	return top;
}

// BugID: 605 -- Select/All followed by Edit/Delete was
// deleting all hidden entities.
//
// 3/26/2003 -- At first release of this implementation, things
// 'appear' to be working okay, but I do not have complete
// confidence.
bool
CSelector::CanSelect( CDbEntity* dbEntity )
{
	CDbFeature*		dbFeature;
	CDbTool*		dbTool;
	EDbEntityType	type;
	int				count, indx;
	
	// Trivial rejection tests.

	type = dbEntity->Type();
#if REQUIRED
	if ( !m_isAllowed[ type ] )
		return FALSE;
#endif

	if ( dbEntity->IsSelected() )
		return FALSE;

	if (m_checkSystemFlag && dbEntity->IsSystem())
		return FALSE;

	if ( dbEntity->IsHidden() )
		return FALSE;

	// Other rejection tests.

	if (type == DBFEATURE)
	{
		dbFeature = (CDbFeature*) dbEntity;
		if ( dbFeature->IsWorkZone() )
		{
			return FALSE;
		}
		else
		{
			count = dbFeature->Count();
			for (indx = 0; indx < count; ++indx)
			{
				if ( !CanSelect( (*dbFeature)[indx] ) )
					return FALSE;
			}
		}
	}
	else if (type == DBPOINT && dbEntity->IsSystem())
	{
		return FALSE;
	}
	else if (type >= DBPOINT && type <= DBCOMMAND)
	{
		dbTool = dbEntity->Tool();
		if (dbTool != NULL && dbTool->IsHidden())
			return FALSE;
	}

	return TRUE;
}
