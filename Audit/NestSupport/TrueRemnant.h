#if !defined(_TRUEREMNANT_H)
#define _TRUEREMNANT_H
#pragma once

// ==================================================================
//		TrueRemnant
//
//	Handy storage area for the various functions used in true-shape
//	remnant building.
//
//	All very special-purpose, with little hope for reuse.
//
// ==================================================================

#include "NestConfig.h"
#include "GeoPoly.h"
#include "WmChain.h"
#include "Sheet.h"
#include "ViewMgr.h"

// ==================================================================

class dllExport CTrueRemnant
{
public:

	CTrueRemnant( const CNestConfig& config, const CViewMgr& view );

	virtual ~CTrueRemnant();

	void TrueShape( const CSheet& sheet );

	void Square( const C3dBox& extent, double cutback );

	void Extent( const C2dBox& extent );

	void Extract( CModel* model, const C3dVec& shift );

	C2dBox Extent( void )		{ return m_remnant.Extent(); }

	double Area( void ) const	{ return m_remnant.Area(); }

private:

	void PartPolysGet( const CNestedArea& nested_area, CWmChainList* part_chains );
	void PartsRemove( const CSheet& sheet, CGeoPoly* material );

	void ContainmentDetermine( CGeoPolyArray& polys );
	void AcceptablePolysMark( CGeoPolyArray& polys );

private:

	// Disabled.
	CTrueRemnant();
	CTrueRemnant( const CTrueRemnant& );
	const CTrueRemnant& operator = ( const CTrueRemnant& );
	int operator == ( const CTrueRemnant& ) const;
	int operator != ( const CTrueRemnant& ) const;

	void	extract_max( CGeoPoly* multipoly, double tol );

	void	generate_offset( CGeoPoly* poly, int dir, double dist );

private:

	CGeoPoly	m_remnant;

	const CNestConfig* m_config;
	const CViewMgr* m_view ;
};

#endif

