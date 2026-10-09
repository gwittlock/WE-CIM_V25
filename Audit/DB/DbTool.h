
#ifndef _DBTOOL_H
#define _DBTOOL_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "ToolShape.h"
#include "GeoCurve.h"
#include "DbEntity.h"

#include "GeoPoly.h"

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class CEntityDb;

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

enum eMatch
{
	MATCH_CRIT,	// required (critical)
	MATCH_PREF,	// preferred
	MATCH_OPT	// optional
};
typedef struct 
{
	char*	m_name;
	eMatch	m_match;
} sMatchType;

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class dllExport CDbTool : public CDbEntity
{
	friend class CEntityDb;

public:

	// Poor man's RTTI.
	virtual EDbEntityType Type() const	{ return DBTOOL; }

	virtual CDbTool* Tool() const			{ return NULL; }

	virtual void Tool( CDbTool* dbTool )	{ dbTool;  /* compiler fodder, do nothing */ }

	virtual C3dBox Box( ID workplaneId ) const;

	// Obtain the list of entities that are referenced by this entity.
	// The entities are appended to the end of the given list.
	virtual void RefsTo( CDbEntityList* list ) const;

	// Determine whether this entity references the given entity.
	virtual bool HasRefTo( const CDbEntity* refdEntity ) const;

	// Returns the same list as RefsTo().
	virtual void Subordinates( CDbEntityList* list ) const;

	virtual void Delete();

	virtual void Accept( CDbEntityVisitor* visitor );

	// Get the display representation of this entity.
	// Note that Tools have a DIFFERENT Describe2d call -- used internally
	// by other entities.
	virtual bool canDescribe2d( void ) const { return FALSE; }
	virtual void Describe2d( 
		int					regen, 
		const C3x4Matrix&	in_ref2world,			// Local plane of reference
		const C3dCoord&		in_localtip,			// Tooltip, in LOCAL REF!!!
		const C2dUnitVec&	in_tangent,				// Tangent along entity...
		bool				in_codePartProf,
		int					in_cutside,
		double				in_tolerance,
		CDisplayEntity*		dispent ) const;

	C3dVec	AggregateOffset( void );

	double EffectiveDiameter() const;
	double EffectiveLength() const;

	eToolSymmetry	Symmetry() const;

	bool IsLayer() const;
	bool IsStockLayer() const;
	bool IsMachineLayer() const;
	bool IsOpenStation() const;
	bool IsHoleTool() const;
	bool IsRoundTool() const;
	bool IsFormTool() const;
	bool IsPunchTool() const;
	bool IsIndexable() const;
	bool IsLeadTool() const;
	bool IsThruTool() const;
	bool IsCuttingTool() const;
	bool IsGapTool() const;

	void PointRepClear();

	// Convert the 2d representation of this tool to planar geometry.
	void Convert( CGeoCurveArray* geoCurves ) const;
	void Convert( CGeoPoly* geoPoly ) const;

	int	Matches( const CDbTool& list_tool ) const;

	CReturn CustomToolInit( const CString& path );

	virtual CReturn CopyTo( CEntityDb* io_dest, tDbEntityMap* io_refmap, CDbEntity** dbEntity );

	// For debugging.
	void Trace() const;

private:  // Methods

	CDbTool( CEntityDb* db );

	CDbTool( const CDbTool& dbTool );

	const CDbTool& operator = ( const CDbTool& dbTool );

	virtual ~CDbTool();

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

	void PointRepUpdate();

	void CutsideIndicator( 
		int					regen, 
		const C3x4Matrix&	in_ref2world,
		const C3dCoord&		in_pc,
		const C2dUnitVec&	in_normal,
		double				in_radius,
		CDisplayEntity*		dispent ) const;

	double IndexAngle( const C2dUnitVec& vec ) const;

private:  // Disabled

	CDbTool();
	int operator == ( const CDbTool& ) const;
	int operator != ( const CDbTool& ) const;

private:  // Data

	C3dVec		agg_off;		// Aggregate head offset vector; defined at use

	CToolShape	m_shape;
};

#endif

