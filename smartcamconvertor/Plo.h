
#ifndef _PLO_H
#define _PLO_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Return.h"
#include "VarList.h"


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class dllExport CPlo
{
public:

	CPlo( const CString& name, int id, const CVarList& nvus );

	int Id()  { return m_id; }

	int IntGet( const CString& name, int defval ) const;

	double DoubleGet( const CString& name, double defval ) const;

	CString StringGet( const CString& name, const CString& defval ) const;

	CVarList& Nvus() { return m_nvus; }

	virtual ~CPlo();

protected:

private:

	// Disabled.
	CPlo();
	CPlo( const CPlo& );
	const CPlo& operator = ( const CPlo& );
	int operator == ( const CPlo& ) const;
	int operator != ( const CPlo& ) const;

private:

	CString		m_name;
	int			m_id;
	CVarList	m_nvus;
};

#endif

