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

#include "stdafx.h"

#include "ColorConst.h"
#include "MathConst.h"
#include "VarList.h"

static int VarCompareFunc( const void* myName, const void* arrayItem );


static CVarList g_empty_varlist;

// ==================================================================

CVarList::CVarList()
{
}

CVarList::CVarList( const CVarList& in_varlist )
{
	(*this) = in_varlist;
}

CVarList::~CVarList()
{
	Reset();
}

const CVarList& CVarList::operator = ( const CVarList& in_varlist )
{
	Reset();

	int	count = in_varlist.countVar();
	for (int indx = 0; indx < count; ++indx)
	{
		CVar* var = in_varlist.getVar( indx );
		newVar( (*var) );
	}

	return (*this);
}

const CVarList& CVarList::operator += ( const CVarList& in_varlist )
{
	int	att_num = in_varlist.countVar();

	for (int att_idx=0; att_idx<att_num; att_idx++)
	{
		CVar* var = in_varlist.getVar( att_idx );
		setVar( *var );
	}

	return (*this);
}

// Wipe the slate clean, deleting all variables.
void CVarList::Reset( void )
{
	m_var_list.DestructiveFlush();
}

CVar* CVarList::getVar( const CString& name ) const
{
	int indx = find( name );
	return ((indx >= 0) ? getVar( indx ) : NULL);
}

// ==================================================================
//	Find the variable of the given name, and return the associated
//	value.  Returns ERROR if the name is not found.
//
CReturn CVarList::getInt( const CString& name, int* io_val ) const
{
	CVar* var = getVar( name );

	if (!var)
	{
		return CReturn( STATUS_ERROR );
	}

	*io_val = var->getInt();
	return CReturn( STATUS_OKAY );
}

CReturn CVarList::getReal( const CString& name, double* io_val) const
{
	CVar* var = getVar( name );

	if (!var)
	{
		return CReturn( STATUS_ERROR );
	}

	*io_val = var->getReal();
	return CReturn( STATUS_OKAY );
}

CReturn CVarList::getString( const CString& name, CString* io_val ) const
{
	CVar* var = getVar( name );
	if (!var)
		return CReturn( STATUS_ERROR );

	(*io_val) = var->getString();

	return CReturn( STATUS_OKAY );
}

void CVarList::setColor( int color )
{
	setInt( "color", color );
}

int CVarList::getColor( int defaultValue ) const
{
	return ( getInt( "color", defaultValue ) );
}

int CVarList::getInt( const CString& name, int defaultValue ) const
{
	int value = defaultValue;
	getInt( name, &value );
	return value;
}

double CVarList::getReal( const CString& name, double defaultValue ) const
{
	double value = defaultValue;
	getReal( name, &value );
	return value;
}

CString CVarList::getString( const CString& name, const CString& defaultValue ) const
{
	CString value = defaultValue;
	getString( name, &value );
	return value;
}

// ==================================================================
//	Set the value of a variable.  If the variable does not exist,
//	then create a new one of the given type and value.
//
//	overall 2 sec
//	0 sec in get
//	1 sec in set
//
void CVarList::setInt( const CString& name, int ival )
{
	int indx;
	bool found = m_var_list.BinarySearch( (void*) &name, &VarCompareFunc, &indx );

	CVar* var;
	if ( found )
		var = getVar( indx );
	else
		var = insertVar( indx, new CVarInt( name ) );

	var->setInt( ival );
}

void CVarList::setReal( const CString& name, double dval )
{
	int indx;
	bool found = m_var_list.BinarySearch( (void*) &name, &VarCompareFunc, &indx );

	CVar* var;
	if ( found )
		var = getVar( indx );
	else
		var = insertVar( indx, new CVarReal( name ) );

	var->setReal( dval );
}

void CVarList::setString( const CString& name, const CString& sval )
{
	int indx;
	bool found = m_var_list.BinarySearch( (void*) &name, &VarCompareFunc, &indx );

	CVar* var;
	if ( found )
		var = getVar( indx );
	else
		var = insertVar( indx, new CVarString( name ) );

	var->setString( sval );
}

// The methods newInt(), newRead(), newString() are used when reading an MM2 file.
// In said case, the vars are guaranteed to be unique and lexically ordered.
void CVarList::newInt( const CString& name, int ival )
{
	CVarInt* var = new CVarInt( name );
	var->setInt( ival );
	m_var_list.Append( var );
}

void CVarList::newReal( const CString& name, double dval )
{
	CVarReal* var = new CVarReal( name );
	var->setReal( dval );
	m_var_list.Append( var );
}

void CVarList::newString( const CString& name, const CString& sval )
{
	CVarString* var = new CVarString( name );
	var->setString( sval );
	m_var_list.Append( var );
}

CReturn CVarList::newVar( const CVar& var ) 
{
	switch (var.getType())
	{
	case VAR_NONE:
		return CReturn( STATUS_ERROR );

	case VAR_INT:
		newInt( var.getName(), var.getInt() );
		break;

	case VAR_REAL:
		newReal( var.getName(), var.getReal() );
		break;

	case VAR_STRING:
		newString( var.getName(), var.getString() );
		break;
	}

	return CReturn( STATUS_OKAY );
}

// ==================================================================
//	Clone a variable here...
CReturn CVarList::setVar( const CVar& var ) 
{
	switch (var.getType())
	{
	case VAR_NONE:
		return CReturn( STATUS_ERROR );

	case VAR_INT:
		setInt( var.getName(), var.getInt() );
		break;

	case VAR_REAL:
		setReal( var.getName(), var.getReal() );
		break;

	case VAR_STRING:
		setString( var.getName(), var.getString() );
		break;
	}

	return CReturn( STATUS_OKAY );
}

void CVarList::deleteVar( const CString& name ) 
{
	int indx = find( name );

	if (indx >= 0)
		deleteVar( indx );
}

void CVarList::deleteVar( int idx ) 
{
	delete m_var_list.Remove( idx );
}



// ==================================================================
//	Debugging diagnostic
void CVarList::Dump( void ) const
{
	CReturn	msg;
	CString	note;

	for (int idx=0; idx<countVar(); idx++)
	{
		CVar* var = getVar( idx );
		note.Format( "Var %d '%s' = '%s'", idx, var->getName(), var->getString() );
		msg.Diagnostic( note );
	}
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Finds the position of a var by name.
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// When found:
//     indx provides position where found
// When not found
//     indx provides the insert position
//
// NOTE: The return value of indx is based on the implementation of CArray.
//
int CVarList::find( const CString& name ) const
{
	int indx;
	bool found = m_var_list.BinarySearch( (void*) &name, &VarCompareFunc, &indx );
	return (found ? indx : -1);
}

CVar* CVarList::insertVar( int indx, CVar* var )
{
	if (indx > countVar())
		m_var_list.Append( var );
	else
		m_var_list.InsertBefore( ((indx < 0) ? 0 : indx), var );

	return var;
}

int VarCompareFunc( const void* myName, const void* arrayItem )
{
	CString* name = ((CString*) myName);
	CVar* theVar = ((CVar*) arrayItem);

	return ( ISGN(theVar->getName().CompareNoCase( (*name) )) );
}

CVar* CVarList::Remove( const CString& name )
{
	int indx = find( name );

	CVar* var = ((indx >= 0) ? m_var_list.Remove( indx ) : NULL);
	
	return var;
}

CVar* CVarList::Remove( int indx )
{
	return ( m_var_list.Remove( indx ) );
}

CReturn CVarList::Insert( CVar* var )
{
	const CString& name = var->getName();

	int indx;
	bool exists = m_var_list.BinarySearch( (void*) &name, &VarCompareFunc, &indx );
	if ( !exists )
		insertVar( indx, var );
	
	return ( CReturn( (exists ? STATUS_ERROR : STATUS_OKAY) ) );
}

void CVarList::VarRename( const CString& oldName, const CString& newName )
{
	CVar* var = Remove( oldName );
	if (var != NULL)
	{
		var->setName( newName );
		Insert( var );
	}
}

const CVarList& CVarList::Bogus()
{
	return g_empty_varlist;
}
