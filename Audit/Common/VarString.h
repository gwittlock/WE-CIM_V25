#if !defined(_VARSTRING_H)
#define _VARSTRING_H

// ==================================================================
//		VarString
//
//	String Variable
//
// ==================================================================

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// ==================================================================

#include "Var.h"

// ==================================================================

class dllExport CVarString : public CVar
{
public:

	CVarString();
	CVarString( const CString& name ) : CVar( name ) { }

	eVarType getType( void ) const			{ return VAR_STRING; }

	void	setInt( int in_val )					{ m_value.Format( "%d", in_val ); }
	void	setReal( double in_val )				{ m_value.Format( "%-12.7f", in_val ); }
	void	setString( const CString& in_val )		{ m_value = in_val; }

	int		getInt( void ) const					{ return atoi( m_value ); }
	double	getReal( void ) const					{ return atof( m_value ); }
	CString	getString( void ) const					{ return m_value; }

	virtual ~CVarString();

protected:

private:
	// Disabled.
	CVarString( const CVarString& );
	const CVarString& operator = ( const CVarString& );
	int operator == ( const CVarString& ) const;
	int operator != ( const CVarString& ) const;

private:
	CString		m_value;
};

#endif

