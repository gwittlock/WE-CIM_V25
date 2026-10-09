#if !defined(_VAR_H)
#define _VAR_H

// ==================================================================
//		Var
//
//	Variable base class
//
//	Sub-classing specific variable types from Var may be a bit
//	excessive, but I'm exploring the concept as an educational
//	experience.  If it sucks in the final, it would be easy 
//	enough to change.
//
// ==================================================================

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


// ==================================================================

enum eVarType
{
	VAR_NONE,
	VAR_INT,
	VAR_REAL,
	VAR_STRING
};

// ==================================================================

class dllExport CVar
{
public:

	CVar();
	CVar( const CString& name );

	virtual eVarType getType( void ) const				{ return VAR_NONE; }

	void	setName( const CString& in_name )			{ m_name = in_name; }
	const CString& getName( void ) const				{ return m_name; }

	virtual void	setInt( int in_val ) = NULL;
	virtual void	setReal( double in_val ) = NULL;
	virtual void	setString( const CString& in_val ) = NULL;

	virtual int		getInt( void ) const = NULL;
	virtual double	getReal( void ) const = NULL;
	virtual CString	getString( void ) const = NULL;

	virtual ~CVar();

protected:

private:
	// Disabled.
	CVar( const CVar& );
	const CVar& operator = ( const CVar& );
	int operator == ( const CVar& ) const;
	int operator != ( const CVar& ) const;

private:

	CString		m_name;
};

#endif

