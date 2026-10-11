
#ifndef _JofDB_H
#define _JofDB_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef _MYDAODB_H
#include "MyDaoDB.h"
#endif

#ifndef _VARLIST_H
#include "VarList.h"
#endif

#ifndef _RETURN_H
#include "Return.h"
#endif


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class dllExport CJofDB
{
public:

	CJofDB();

	CReturn Create( const CString& filepath );

	CReturn Open( const CString& filepath );

	void Close();

	CReturn Update( const CVarList& attribs );

	virtual ~CJofDB();

protected:

private:  // Methods

	CReturn TablesCreate( const CString& filepath );

	CReturn ToolCribCreate();

	int ToolCribAdd( const CVarList& attribs );

	void IndicesInvalidate();

private:  // Disabled

	CJofDB( const CJofDB& );
	const CJofDB& operator = ( const CJofDB& );
	int operator == ( const CJofDB& ) const;
	int operator != ( const CJofDB& ) const;

private:  // Data

	CMyDaoDB	m_db;

	int		m_tblToolCrib;
};

#endif

