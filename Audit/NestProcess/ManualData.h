#if !defined(_MANUALDATA_H)
#define _MANUALDATA_H
#pragma once

// ==================================================================
//		ManualData
//
// ==================================================================

#include "Common.h"

#include "NestConfig.h"
#include "Sheet.h"
#include "PartBin.h"

// ==================================================================

class CManualData
{
public:
	CManualData();
	virtual ~CManualData();

	CReturn InitSheet( CString& configdb, CModel* model, CViewMgr& view );

	int UsePattern( ID id, const C3dCoord& place, CViewMgr& view );
	int UseInstance( ID id, CViewMgr& view );
	int UseSelection( const C3dCoord& place, CViewMgr& view );
	int UseFile( const CString& filepath, const C3dCoord& place, CViewMgr& view );

	CReturn Bump( const C3dVec& delta, CViewMgr& view );
	CReturn Punch( CViewMgr& view );

	void Position( const C3dCoord& pos )	{ m_place = pos; }
	C3dCoord Position( void ) const			{ return m_place; }

	CDbCommand* Instance( void ) const		{ return m_instance; }

protected:

private:
	// Disabled.
	CManualData( const CManualData& );
	const CManualData& operator = ( const CManualData& );
	int operator == ( const CManualData& ) const;
	int operator != ( const CManualData& ) const;

private:
	void reset_part( void );

private:
	// General setup (sheet) information
	//
	CModel*		m_model;

	CNestConfig	m_config;
	CSheet		m_sheet;
	CPartBin	m_partbin;
	//
	// This data relates to the pattern (part) being nested on the sheet
	//
	CNestingPart*	m_part;
	C3dCoord		m_place;
	CDbCommand*		m_instance;
};

#endif

