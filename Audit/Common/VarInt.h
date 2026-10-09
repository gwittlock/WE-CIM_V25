#if !defined(_VARINT_H)
#define _VARINT_H

// ==================================================================
//		VarInt
//
//	Integer Variable
//
// ==================================================================

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// ==================================================================

#include "Var.h"

// ==================================================================

class dllExport CVarInt : public CVar
{
public:
	
	CVarInt();
	CVarInt( const CString& name ) : CVar( name ) { }

	eVarType getType( void ) const			{ return VAR_INT; }

	void	setInt( int in_val )					{ m_value = in_val; }
	void	setReal( double in_val )				{ m_value = (int) in_val; }
	void	setString( const CString& in_val )		{ m_value = atoi( (LPCSTR) in_val ); }

	int		getInt( void ) const					{ return m_value; }
	double	getReal( void ) const					{ return (double)m_value; }
	CString	getString( void ) const					{ CString tmpstr; tmpstr.Format( "%d", m_value ); return tmpstr; }

	virtual ~CVarInt();

protected:

private:
	// Disabled.
	CVarInt( const CVarInt& );
	const CVarInt& operator = ( const CVarInt& );
	int operator == ( const CVarInt& ) const;
	int operator != ( const CVarInt& ) const;

private:
	int		m_value;
};

#endif

