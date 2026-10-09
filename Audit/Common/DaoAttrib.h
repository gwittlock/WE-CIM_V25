#if !defined(_DAOATTRIB_H)
#define _DAOATTRIB_H
#pragma once

// ==================================================================
//		DaoAttrib
// ==================================================================
//
//	For easy access to tables that use the attribute format --
//	extract a value from the CValue field given the CVarName.
//
//	This is a convenience class tying together DaoDb and DaoQuery
//
// ==================================================================

#include "Common.h"
#include "DaoDB.h"
#include "DaoQuery.h"


// ==================================================================

class dllExport CDaoAttrib
{
public:

	CDaoAttrib( CDaoDB* database, const CString& table );
	virtual ~CDaoAttrib();

	int	Int( const CString& name, int def );
	double Double( const CString& name, double def );
	CString String( const CString& name, const CString& def );

	int	ciInt( const CString& name, int def );
	double ciDouble( const CString& name, double def );
	CString ciString( const CString& name, const CString& def );

	int	ciInt( const CString& name, const CString& idName, int id, int def );
	double ciDouble( const CString& name, const CString& idName, int id, double def );
	CString ciString( const CString& name, const CString& idName, int id, const CString& def );

private:
	// Disabled.
	CDaoAttrib( const CDaoDB& );
	const CDaoAttrib& operator = ( const CDaoAttrib& );
	int operator == ( const CDaoAttrib& ) const;
	int operator != ( const CDaoAttrib& ) const;

private:

	CDaoDB*	m_db;
	CString m_table;
};

#endif

