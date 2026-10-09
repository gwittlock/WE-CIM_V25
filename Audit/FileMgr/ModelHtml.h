
#ifndef _MODELHTML_H
#define _MODELHTML_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef _RETURN_H
#include "Return.h"
#endif

class CModel;


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class dllExport CModelHtml
{
public:

	CModelHtml();

	CReturn Dump( const CString& filepath, const CModel& model );

	virtual ~CModelHtml();

protected:

private:  // Methods

	static void VarListDump( FILE* f, const CVarList& varlist, const CString& title );
	static void HighLevelDump( const CDbEntity& dbEntity, FILE* f );
	static void CanonicalDump( const CDbEntity& dbEntity, FILE* f );
	static void AttributesDump( const CDbEntity& dbEntity, FILE* f );
	static void ReferencesDump( const CDbEntity& dbEntity, FILE* f );
	static void OwnsDump( const CDbEntity& dbEntity, FILE* f );
	static CString EntityDescription( const CDbEntity& dbEntity );

	static BOOL CanReference( const CDbEntity& dbEntity );
	static BOOL IsContainer( const CDbEntity& dbEntity );
	static BOOL HasCanonicalData( const CDbEntity& dbEntity );

private:  // Disabled

	CModelHtml( const CModelHtml& );
	const CModelHtml& operator = ( const CModelHtml& );
	int operator == ( const CModelHtml& ) const;
	int operator != ( const CModelHtml& ) const;

private:  // Data

};

#endif

