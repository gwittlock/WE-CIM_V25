
#ifndef _DBFEATURE_H
#define _DBFEATURE_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "DynamicArray.h"
#include "DbEntity.h"
#include "DbContainer.h"

class CGeoElem;
class CEntityDb;


// ============================================================================
//
// Feature subtypes... to replace use of the "_type" or STR_TYPE attribute.
// NOT CURRENTLY USED
//
// Note that Patterns are a type of feature, but defined as a subclass instead
// of a subtype.
//
enum EFeatureType
{
	FEATURE_GENERIC=0,
	FEATURE_ZONE,
	FEATURE_PART,
	FEATURE_LEAD,
};

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// A feature can be either a 'part feature' or a 'manufacturing feature'.
// In either case, a feature can contain subfeatures.  When a feature is
// deleted, all of its subfeatures will be deleted as well.  If you do
// not want to delete a subfeature, it must first be removed from the
// feature before deleting the feature.
//
// BEWARE: Current policy dictates that features contain only part or
// toolpath entities, but not both.  There is no enforcement of this
// policy, therefore the client must be responsible.
//
class dllExport CDbFeature : public CDbContainer
{
	friend class CEntityDb;

public:

	// Poor man's RTTI.
	virtual EDbEntityType Type() const   { return DBFEATURE; }

	EFeatureType SubType() const		{ return m_subtype; }
	void SubType( EFeatureType type )	{ m_subtype = type; }

	virtual CDbWorkplane* Workplane() const;
	virtual CDbTool* Tool() const;

	virtual void Workplane( CDbWorkplane* workplane );

	virtual void Tool( CDbTool* dbTool  )	{ dbTool;  /* compiler fodder, do nothing */ }

	// Return the count of immediate children.
	virtual int Count() const;

	// Get the ith immediate child entity.
	virtual CDbEntity* GetAt( int indx ) const;
	virtual CDbEntity* operator[]( int indx ) const;

	virtual CDbEntity* ReplaceAt( int indx, CDbEntity* dbEntity );

	// Make the given entity a child of this feature.
	virtual CReturn Prepend( CDbEntity* dbEntity, bool copy = FALSE );

	// Make the given entity a child of this feature.
	virtual CReturn Append( CDbEntity* dbEntity, bool copy = FALSE );

	// Insert an entity in front of a known entity in this container.
	virtual int InsertBefore( CDbEntity* refEntity, CDbEntity* newEntity );
	virtual int InsertBefore( int indx, CDbEntity* newEntity );

	// Insert an entity behind a known entity in this container.
	virtual int InsertAfter( CDbEntity* refEntity, CDbEntity* newEntity );
	virtual int InsertAfter( int indx, CDbEntity* newEntity );

	// Find the position of the given curve in the profile (0 indexed).
	virtual int Position( const CDbEntity* refEntity ) const;

	// Reorder an entity within the feature
	CReturn	MoveBefore( CDbEntity* dstEntity, CDbEntity* srcEntity );
	CReturn	MoveAfter( CDbEntity* dstEntity, CDbEntity* srcEntity );

	// CDbContainer requirement -- releases ownership of the given entity.
	// Returns true if successful.
	virtual bool Disown( CDbEntity* dbEntity );

	virtual void BenignFlush();
	virtual CReturn DestructiveFlush();

	CReturn RefsFlush();

	void AddRef( CDbEntity* dbEntity );

	// Obtain a list of entities that this feature references.
	virtual void RefsTo( CDbEntityList* list ) const;

	// Determine whether this feature has a reference to the given entity.
	virtual bool HasRefTo( const CDbEntity* refdEntity ) const;

	// Obtain a list of entities owned by this entity.
	virtual void Subordinates( CDbEntityList* list ) const;

	// Get the atomic entities.
	virtual void Flatten( CDbEntityList* entities ) const;

	virtual void Delete();

	virtual void Accept( CDbEntityVisitor* visitor );

	// Obtain the bounding box containing all feature entities.
	virtual C3dBox Box( ID workplaneId = ~0 ) const;

	virtual void	Transform( const C3x4Matrix& in_xform );

	// Convenience methods.

	bool IsWorkZone() const;
	bool IsPart() const;
	bool IsLead() const;
	bool IsLeadIn() const;
	bool IsLeadOut() const;

	void HoldDownUpdate( const CVarList& model_header );

	// This is public because of CDbEntity::RubberStart()
	void DescribeHold(
		int			regen,
		int			hold,
		C3dCoord*	endpt,
		double		in_tolerance ) const;

	void WriteClampsToFile( const CString& path ) const;

public:

	// 2004.12.24 (PE) -- Added CTG support for clamps and holddowns.
	static CReturn ClampInitFromCTG( const CString& ctg_path );
	static CReturn HoldInitFromCTG( const CString& ctg_path );

	// 2007.03.04 (PE) -- To enable update via Edit/Project.
	static void HoldDownInvalidate();

	static const CToolShape& HoldDownShape();

protected:

	CDbFeature( const CDbFeature& dbFeature );

	CDbFeature( CEntityDb* db );

	virtual ~CDbFeature();

	const CDbFeature& operator = ( const CDbFeature& dbFeature );

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

	virtual bool canDescribe2d( void ) const { return TRUE; }
	virtual void Describe2d( int regen, C3dCoord* io_tooltip, double in_tolerance ) const;

private:

	void DescribeAccessories( 
		int			regen,
		C3dCoord*	io_tooltip,
		double		tolerance ) const;

	void DescribeZone(
		int			regen,
		C3dCoord*	endpt,
		double		in_tolerance ) const;

	void DescribeClamp(
		int			regen,
		int			clamp,
		C3dCoord*	endpt,
		double		in_tolerance ) const;

	void ClampInitDefault() const;
	void HoldInitDefault() const;

private:  // Disabled

	CDbFeature();
	int operator == ( const CDbFeature& ) const;
	int operator != ( const CDbFeature& ) const;

private:  // Data

	// The reference entities that are used to
	// create the entities owned by this feature.
	CDbEntityList m_refd;

	// The entities owned by this feature.
	CDbEntityList m_list;

	// Formalized feature sub-types... 
	EFeatureType	m_subtype;
};

typedef CDynamicArray<CDbFeature*> CDbFeatureArray;

#endif

