#ifndef _ENTITYCOPIER_H
#define _ENTITYCOPIER_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "DbAllEntities.h"
#include "DbEntityVisitor.h"
#include "EntityDb.h"


// ==================================================================

class dllExport CEntityCopier : public CDbEntityVisitor
{
public:

	CEntityCopier();

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Use this method if you are copying entities and
	// DO NOT need to retain the mapping between entities.
	//
	// dst_db -- the recipient database for copied entities.
	// flags -- FLAG_NONE, FLAG_COPY_TOOL_BY_CONTENT (common\CommonFlags.h)
	void Init( CEntityDb* dst_db, BYTE flags );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Use this method if you are copying entities and
	// DO need to retain the mapping between entities.
	//
	// dst_db -- the recipient database for copied entities.
	//
	// forward_reference_map -- map between the original and
	//    the copied entities.  Can be NULL.
	//
	// reverse_reference_map -- map between the copied and
	//    the original entities.  Can be NULL
	//
	// NOTE: An alternate implementation would be to provide
	// a single reference map and a behavior flag that controls
	// what is added to the reference map.
	//
	void Init(
				CEntityDb*		dst_db,
				tDbEntityMap*	forward_reference_map,
				tDbEntityMap*	reverse_reference_map,
				BYTE			flags );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// For traversal-type usage.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	virtual void Visit( CDbEntity* dbEntity );

	CDbEntity* ForwardLookup( CDbEntity* theSource );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// For low-level usage.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	CDbWorkplane*	WorkCopy( CDbWorkplane* dbWork );

	CDbTool*		ToolCopy( CDbTool* dbTool );

	CDbPoint*		PointCopy( CDbPoint* dbPoint );

	CDbLine*		LineCopy( CDbLine* dbLine );

	CDbArc*			ArcCopy( CDbArc* dbArc );

	CDbHole*		HoleCopy( CDbHole* dbHole );

	CDbCommand*		CommandCopy( CDbCommand* dbCommand );

	CDbProfile*		ProfileCopy( CDbProfile* dbProfile );

	CDbFeature*		FeatureCopy( CDbFeature* dbFeature );

	CDbSequence*	SequenceCopy( CDbSequence* dbSequence );

	CDbPattern*		PatternCopy( CDbPattern* dbPattern );

	virtual ~CEntityCopier();

private:

	void			ConditionalAddToContainer( CDbEntity* theOriginal, CDbEntity* theCopy );

	void			EntityMapUpdate( CDbEntity* theOriginal, CDbEntity* theCopy );

private:

	// Disabled.
	CEntityCopier( const CEntityCopier& );
	const CEntityCopier& operator = ( const CEntityCopier& );
	int operator == ( const CEntityCopier& ) const;
	int operator != ( const CEntityCopier& ) const;

private:

	tDbEntityMap	m_forward_reference_map;

	tDbEntityMap*	m_fwd_ptr;
	tDbEntityMap*	m_rev_ptr;

	BYTE			m_flags;
	CEntityDb*		m_dst_db;
};

#endif

