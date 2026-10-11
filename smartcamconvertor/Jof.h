
#ifndef _JOF_H
#define _JOF_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Return.h"
#include "VarList.h"
#include "Plo.h"
#include "DynamicArray.h"


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class dllExport CJof
{
public:

	CJof();

	CReturn Read( const CString& filepath );

	CReturn DbCreate( const CString& filepath );

	int StepsCount() const;
	int ToolsCount() const;

	void ToolAttribsGet( int indx, CVarList* attribs ) const;

	// For populating.

	void NvuAdd( const char* name, const char* value );

	void PloCreate( const char* name, int id );

	int	ToolAssignmentGet( int step_num ) const;

	// For debugging.

	void AttribsDump( const CString& filepath );

	void ToolsDump( const CString& filepath );

	virtual ~CJof();

protected:

private:  // Methods

	CString* Load( const CString& filepath );

	void PlosFind( const CString& ploType, int* start, int* count );
	void AttribsGet( int tassign_indx, CVarList* attribs );

	void PlosConvert();
	void AttribsConvert( const CVarList& superset );

	void CondKerfAdd( CVarList* attribs );
	bool IsReqdOP( int toolsncs, CPlo* op );

	void JOF_Parse( const CString& filepath );
	void PLO_Parse( FILE* f, const CString& text );
	bool IsRecognizedPLO( const CString& text);
	void StringPairGet( const CString& text, CString* name, CString* value );

private:  // Disabled

	CJof( const CJof& );
	const CJof& operator = ( const CJof& );
	int operator == ( const CJof& ) const;
	int operator != ( const CJof& ) const;

private:  // Data

	CVarList				m_nvus;

	CDynamicArray<CPlo*>	m_steps;
	CDynamicArray<CPlo*>	m_tool_assignments;
	CDynamicArray<CPlo*>	m_tool_assemblies;
	CDynamicArray<CPlo*>	m_tools;
	CDynamicArray<CPlo*>	m_ops;

	CDynamicArray<CVarList*>	m_tool_attribs;
};

#endif

