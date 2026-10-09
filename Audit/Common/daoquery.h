
#ifndef _DAOQUERY_H
#define _DAOQUERY_H


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// ==================================================================

#ifndef _VARLIST_H
#include "VarList.h"
#endif

#ifndef _DAODB_H
#include "DaoDB.h"
#endif

class CReturn;
class CDaoDatabase;
class CDaoQueryDef;
class CDaoRecordset;


// ==================================================================
// NOTE: In our system, database field names always use ' ' to
// separate words in a field name.  In the C++ code, convention
// dictates that we use '_' to separate words, however.  This
// latter convention exists primarily to simplify the command
// parser (CCommand) and attribute names (CVar).  As such,
// this class incurs a little overhead to convert ' ' to '_'.

#define DAO_NO_FLAGS				0x000
#define DAO_REPLACE_WHITE_SPACE		0x001
#define DAO_IGNORE_COMPOUND_NAMES	0x002


class dllExport CDaoQuery
{
public:

	CDaoQuery();

	CReturn Init( CDaoDatabase* db, const CString& sqlQuery );

	void Terminate(void);

	// Gets the count of records from the query results.
	int RecordCount();

	// Gets the count of fields in the current record.
	int FieldCount();

	// (-) backward one record / (0) to start / (+) forward one record
	CReturn Move( int dir );

	CReturn MoveFirst();
	CReturn MoveLast();

	// Another type of Move() method that uses a query string.
	//
	// Values for lFindType:
	//   AFX_DAO_NEXT   Find the next location of a matching string.
	//   AFX_DAO_PREV   Find the previous location of a matching string.
	//   AFX_DAO_FIRST  Find the first location of a matching string.
	//   AFX_DAO_LAST   Find the last location of a matching string. 
	//
	// Returns (true) found a matching record / (false) failed to find a matching record.
//	bool Find( long lFindType, LPCTSTR lpszFilter );

	// Gets the name of the current field.
	CString FieldNameGet( int indx );

	int IntGet( int indx );
	int IntGet( const CString& fieldName );

	double DoubleGet( int indx );
	double DoubleGet( const CString& fieldName );

	CString StringGet( int indx );
	CString StringGet( const CString& fieldName );

	void AttributesCopy( CVarList* attribs, int flags );

	virtual ~CDaoQuery();

protected:

private:  // Disabled.

	CDaoQuery( const CDaoQuery& );
	const CDaoQuery& operator = ( const CDaoQuery& );
	int operator == ( const CDaoQuery& ) const;
	int operator != ( const CDaoQuery& ) const;

private:

	CDaoDatabase*  m_db;
	CDaoQueryDef*  m_queryDef;
	CDaoRecordset* m_queryResults;
	
	bool m_closeDb;
};

#endif

