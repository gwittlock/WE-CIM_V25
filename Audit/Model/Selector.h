
#ifndef _SELECTOR_H
#define _SELECTOR_H

#ifndef _DBENTITY_H
#include "DbEntity.h"
#endif

class C2dBox;
class C3dBox;
class CGeoLine;
class CModel;

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class dllExport CSelector
{
public:

	// By default, a selector always checks an entitys' system flag,
	// causing the selector to ignore 'system entities'.
	//   See also CSelector::SystemFlag();
	CSelector( CModel& model );

	const CModel& Model( void ) const				{ return *m_model; }

	// const here seems odd but modifying the model via
	// the selector is not the same as modifying the selector.
	CModel*	pModel( void ) const					{ return m_model; }

	// Determine whether the given entity is in the selection set.
	bool IsSelected( ID id ) const;

	// Obtain the number of entities in the selection set.
	int Count() const								{ return m_selected.Count(); }

	// Obtain the Ith entity in the selection set.
	CDbEntity* GetAt( int indx ) const				{ return m_selected[indx]; }
	CDbEntity* operator [] ( int indx ) const		{ return m_selected[indx]; }

	CDbEntity* FirstSelected()						{ return m_firstSelected; }

	// Remove all entities from the selection set.
	void Clear();


	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Selector behavior tuning methods
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	// Set off/on all filters.
	void All( int state );

	// Set off/on the given entity filter.
	void Filter( EDbEntityType type, int state );

	// By default, a selector will select the children of a container
	// regardless of the filter settings.  By turning on restrictions,
	// the selection is limited to those entities that pass the filter
	// settings.
	//
	// For instance, if the filter settings allow only profiles to be
	// selected, the default behavior is to select the profile and all
	// of its curves.  When restrictions are active, only the profile
	// itself will be selected.
	void Restrictions( bool active )	{ m_restricted = active; }

	// Determines whether subsequent uses of this selector
	// ignores 'system' entities (like entities on the stock layer).
	// By default, a selector always checks an entitys' system flag,
	// causing the selector to ignore 'system entities'.
	void SystemFlag( bool state )		{ m_checkSystemFlag = state; }

	// Special override to allow selection of workzones
	// While this is almost certainly violating the spirit of SOMETHING, we
	// *have* to be able to pick the workzone sometimes, so we can drag the
	// repo hold-down.
	void AllowWorkzone(bool state)			{ m_allowWorkzone = state; }

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Entity selection methods
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	// Select the given entity based on the current filter settings.
	// NOTE: This method walks back up the entity hierarchy and 
	// selects top-down.  That is, if a curve is selected, and the
	// curve is owned by a profile, and the selection filter allows
	// profiles to be selected, the profile and all of its children
	// will also be selected.
	//
	// See also Restrictions()
	//
	CReturn FilteredSelect( ID id );
	CReturn FilteredDeselect( ID id );

	// Add & Remove a single entity regardless of filter setting.
	// NOTE: Introduced in support of Java.
	CReturn Add( ID id );
	CReturn Remove( ID id );

	// Select all entities that reference the entity having the
	// given id.  NOTE: The selection filters still apply.
	// Historically, this method was added to allow the selection
	// of all entities on a given layer (for instance).
	CReturn SelectAllRefsTo( ID id );
	CReturn DeselectAllRefsTo( ID id );

	// Select all entities that cross the given bounding
	// box, using the current selection filter.
	void SelectBox( const C2dBox& box, double tol );

	// Select all entities contained in the given bounding
	// box, using the current selection filter, IN WORLD COORDINATES
	void WithinBox( const C3dBox& box, double tol );
	void FeatureWithinBox( const C3dBox& box, double tol );
	
	// Select all entities using the current selection filter.
	void SelectAll( bool considerHiddenStatus );

	// In support of java Selector.StatePop(), returns the
	// current filter settings encoded as an integer.
	int Filters();

	// Moved from private to support internal (as opposed to VB) usage.
	CReturn FilteredSelect( CDbEntity* dbEntity, bool considerHiddenStatus );
	void Add( CDbEntity* dbEntity, bool considerHiddenStatus );

	virtual ~CSelector();

private:  // Methods

	CReturn FilteredDeselect( CDbEntity* dbEntity );

	CReturn Remove( CDbEntity* dbEntity );

	bool IsTarget( const CDbEntity& dbEntity );
	bool NeedOwner( const CDbEntity& dbEntity );

	bool BoxSelection(
					const C2dBox&		box2d,
					const CGeoLine*		boxGeo,
					const CDbEntity*	dbEntity,
					double				tol );
	
	CDbEntity* TopLevelEntityFind( CDbEntity* dbEntity );

	bool CanSelect( CDbEntity* dbEntity );

private:  // Disabled.

	CSelector();
	CSelector( const CSelector& );
	const CSelector& operator = ( const CSelector& );
	int operator == ( const CSelector& ) const;
	int operator != ( const CSelector& ) const;

private:  // Data.

	CModel* m_model;

	// 2005.04.17 (PE) -- Switched from CDbEntityList to CDbEntityArray
	// to simplify debugging.  Also makes it easier to sort the selected
	// entities.
	//   CDbEntityList m_selected;
	CDbEntityArray m_selected;
	CDbEntity* m_firstSelected;

	bool m_isAllowed[ DBTERMINAL ];

	bool m_checkSystemFlag;
	bool m_restricted;
	bool m_allowWorkzone;
};

#endif
