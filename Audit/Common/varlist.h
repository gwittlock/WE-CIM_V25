#if !defined(_VARLIST_H)
#define _VARLIST_H

// ==================================================================
//		VarList
//
//	Variable-List
//
//	Contains a list of variables.  Owns the variables; creates,
//	stores, and deletes them.  A veritable variable factory.
//
//	Provides rapid and easy access to variables in the list.
//
// ==================================================================

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// ==================================================================

#include "VarInt.h"
#include "VarReal.h"
#include "VarString.h"
#include "Return.h"

// ==================================================================

#ifndef _DYNAMICARRAY_H
#include "DynamicArray.h"
#endif


// ==================================================================

class Var;

class dllExport CVarList
{
public:

	CVarList();

	CVarList( const CVarList& in_varlist );

	virtual ~CVarList();

	// Assignment operator
	const CVarList& operator = ( const CVarList& in_varlist );

	// Union operator.
	const CVarList& operator += ( const CVarList& in_varlist );

	void Reset( void );

	// ---------------------------------------------------------------

	CReturn	getInt( const CString& in_name, int* io_val ) const;
	CReturn	getReal( const CString& in_name, double* io_val) const;
	CReturn	getString( const CString& in_name, CString* io_val ) const;

	void setColor( int color );
	int getColor( int defaultValue ) const;

	int getInt( const CString& in_name, int defaultValue ) const;
	double getReal( const CString& in_name, double defaultValue  ) const;
	CString getString( const CString& in_name, const CString& defaultValue ) const;

	void setInt( const CString& in_name, int in_val );
	void setReal( const CString& in_name, double in_val );
	void setString( const CString& in_name, const CString& in_val );

	// Special case var creation for file import...
	void newInt( const CString& in_name, int in_val );
	void newReal( const CString& in_name, double in_val );
	void newString( const CString& in_name, const CString& in_val );

	int		countVar() const							{ return m_var_list.Count(); }
	CVar*	getVar( int in_idx ) const					{ return m_var_list[in_idx]; }
	CVar*	getVar( const CString& in_name ) const;
	CReturn	setVar( const CVar& in_var );

	void deleteVar( const CString& in_name );
	void deleteVar( int in_idx );

	// A low-level support method that is to be used with caution by
	// clients other than a CVarList object.  Moved from private to
	// public in an attempt to reduce overhead in the rendering system.
	int find( const CString& name ) const;

	// Removes the named attribute from the list and returns a pointer to it.
	CVar* Remove( const CString& name );
	CVar* Remove( int indx );

	// Inserts the given var at its proper lexical position (increasing order).
	// Returns an error if a var of the same name already exists in the list.
	CReturn Insert( CVar* var );

	void VarRename( const CString& oldName, const CString& newName );

	// ---------------------------------------------------------------

	void Dump( void ) const;

public:

	static const CVarList& Bogus();

protected:

private: // methods

	CReturn newVar( const CVar& in_var ) ;
	CVar* insertVar( int indx, CVar* var );

private: // Disabled.

	int operator == ( const CVarList& ) const;
	int operator != ( const CVarList& ) const;

private: // data

	CDynamicArray<CVar*> m_var_list;
};

typedef CDynamicArray<CVarList*> tArrayOfVarList;

#endif

