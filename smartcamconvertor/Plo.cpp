
#include "stdafx.h"
#include "Plo.h"

CPlo::CPlo( const CString& name, int id, const CVarList& nvus )
{
	m_name = name;
	m_id = id;
	m_nvus = nvus;
}

CPlo::~CPlo()
{
}

int
CPlo::IntGet( const CString& name, int defval ) const
{
	return m_nvus.getInt( name, defval );
}

double
CPlo::DoubleGet( const CString& name, double defval ) const
{
	return m_nvus.getReal( name, defval );
}

CString
CPlo::StringGet( const CString& name, const CString& defval ) const
{
	return m_nvus.getString( name, defval );
}
