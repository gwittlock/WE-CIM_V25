
#ifndef _WMCRVREC_H
#define _WMCRVREC_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef _GEOCURVE_H
#include "GeoCurve.h"
#endif

#ifndef _WMELEM_H
#include "WmElem.h"
#endif

#ifndef _WMCRVRECEXTDATA_H
#include "WmCrvRecExtData.h"
#endif

class CGeoElem;
class CWmSubchn;


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class dllExport CWmCrvRec
{
public:

	CWmCrvRec( CWmElem* elem );

	CWmCrvRecExtData* Update( const CWmIntRec* rec, double uparam );

	CWmElem* WmElem() const							{ return m_wmelem; }

	CGeoElem* Elem() const							{ return m_wmelem->Elem(); }

	int Count() const								{ return m_list.Count(); }

	CWmCrvRecExtData* ExtData( int indx) const		{ return m_list[indx]; }

	virtual ~CWmCrvRec();

protected:

private:  // Methods

private:  // Disabled

	CWmCrvRec();
	CWmCrvRec( const CWmCrvRec& );
	const CWmCrvRec& operator = ( const CWmCrvRec& );
	int operator == ( const CWmCrvRec& ) const;
	int operator != ( const CWmCrvRec& ) const;

private:  // Data

	CWmElem*			m_wmelem;

	CWmCrvRecExtDataList	m_list;
};

typedef CIndxList<CWmCrvRec*> CWmCrvRecList;

#endif

