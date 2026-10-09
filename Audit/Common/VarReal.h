#if !defined(_VARREAL_H)
#define _VARREAL_H

// ==================================================================
//		VarReal
//
//	Real (double) Variable
//
// ==================================================================

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// ==================================================================

#include "Var.h"

// ==================================================================

class dllExport CVarReal : public CVar
{
public:

	CVarReal();
	CVarReal( const CString& name ) : CVar( name ) { }

	eVarType getType( void ) const					{ return VAR_REAL; }

	void	setInt( int in_val )					{ m_value = (double)in_val; }
	void	setReal( double in_val )				{ m_value = in_val; }
	void	setString( const CString& in_val )		{ m_value = atof( in_val ); }

	int		getInt( void ) const					{ return (int)(m_value); }
	double	getReal( void ) const					{ return m_value; }
	CString	getString( void ) const					{ CString tmpstr; tmpstr.Format( "%f", m_value ); return tmpstr; }

	virtual ~CVarReal();

protected:

private:
	// Disabled.
	CVarReal( const CVarReal& );
	const CVarReal& operator = ( const CVarReal& );
	int operator == ( const CVarReal& ) const;
	int operator != ( const CVarReal& ) const;

private:
	double	m_value;
};

#endif

