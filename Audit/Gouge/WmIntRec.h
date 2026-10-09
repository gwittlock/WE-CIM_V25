
#ifndef _WMINTREC_H
#define _WMINTREC_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef _INDXLIST_H
#include "IndxList.h"
#endif

#ifndef _WMCRVRECEXTDATA_H
#include "WmCrvRecExtData.h"
#endif

class CWmElem;
class CWmSubchn;


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class dllExport CWmIntRec
{
public:

	CWmIntRec( const C2dCoord& pt );

	const C2dCoord& Point() const						{ return m_pt; }

	int Count() const									{ return m_list.Count(); }

	const CWmCrvRecExtData* ExtData( int indx ) const;

	void Update( const CWmCrvRecExtData* rec );

	void Reduce();

	virtual ~CWmIntRec();

	bool IsValid() const;

protected:

private:  // Methods

private:  // Disabled

	CWmIntRec();
	CWmIntRec( const CWmIntRec& );
	const CWmIntRec& operator = ( const CWmIntRec& );
	int operator == ( const CWmIntRec& ) const;
	int operator != ( const CWmIntRec& ) const;

private:  // Data

	C2dCoord m_pt;

	// CCrvRecExtDataList m_list;
	CIndxList<const CWmCrvRecExtData*> m_list;
};

typedef CIndxList<CWmIntRec*> CWmIntRecList;

#endif

