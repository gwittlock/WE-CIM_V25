
#ifndef _MODELTEXT_H
#define _MODELTEXT_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef _RETURN_H
#include "Return.h"
#endif

class CModel;


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class dllExport CModelText
{
public:

	CModelText();

	void Enable( bool enable )	{ m_enabled = enable; }

	CReturn Dump( const CString& filepath, const CModel& model );

	virtual ~CModelText();

protected:

private:  // Methods

	 void Summarize( const CModel& model, FILE* f );

	 void EntityDump( const CDbEntity& dbEntity, FILE* f );
	 void VarListDump( FILE* f, const CVarList& varlist, const CString& title );
	 void AttributesDump( const CDbEntity& dbEntity, FILE* f );
	 void ReferencesDump( const CDbEntity& dbEntity, FILE* f );
	 void OwnsDump( const CDbEntity& dbEntity, FILE* f );
	 void CanonicalDump( const CDbEntity& dbEntity, FILE* f );
	 CString EntityDescription( const CDbEntity& dbEntity );

	 bool CanReference( const CDbEntity& dbEntity );
	 bool HasCanonicalData( const CDbEntity& dbEntity );

	 CString Label( EDbEntityType type );

	 bool CanPrint( const CString& name );

private:  // Disabled

	CModelText( const CModelText& );
	const CModelText& operator = ( const CModelText& );
	int operator == ( const CModelText& ) const;
	int operator != ( const CModelText& ) const;

private:  // Data

	bool m_enabled;
	bool m_is_rtl_running;
};

#endif

