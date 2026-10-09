
#ifndef _CLFILESTATE_H
#define _CLFILESTATE_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef _INDXLIST_H
#include "IndxList.h"
#endif

#ifndef _VARLIST_H
#include "VarList.h"
#endif

#include <afxtempl.h>


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class CClfileState
{
public:

	CClfileState( int recDataCount, int intDataCount, int dblDataCount );

	CReturn Set(
				 const long* recData,
				 const long* intData,
				 const double* dblData,
				 const CVarList& attribs );

	CReturn Get(
				 long* recData,
				 long* intData,
				 double* dblData,
				 CVarList* attribs );

	virtual ~CClfileState();

protected:

private:  // Methods

private:  // Disabled

	CClfileState();
	CClfileState( const CClfileState& );
	const CClfileState& operator = ( const CClfileState& );
	int operator == ( const CClfileState& ) const;
	int operator != ( const CClfileState& ) const;

private:  // Data

	CArray<long,long> m_recData;
	CArray<long,long> m_intData;
	CArray<double,double> m_dblData;
	CVarList m_attribs;
};

typedef CIndxList<CClfileState*> CClfileStateList;

#endif

