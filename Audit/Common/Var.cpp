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

#include "stdafx.h"
#include "Var.h"

// ==================================================================

CVar::CVar()
	: m_name()
{
}

CVar::CVar( const CString& name )
	: m_name( name )
{
}

CVar::~CVar()
{
}


// ==================================================================
