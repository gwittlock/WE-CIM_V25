
#ifndef _INTSCTREC_H
#define _INTSCTREC_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef _INDXLIST_H
#include "IndxList.h"
#endif

#ifndef _3DCOORD_H
#include "3dCoord.h"
#endif

#ifndef _DBCURVE_H
#include "DbCurve.h"
#endif


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class CIntsctRec
{
public:

	CIntsctRec( const C3dCoord& pt, const CDbCurve* dbCurveA, const CDbCurve* dbCurveB )
		: m_pt( pt ), m_dbCurveA( dbCurveA ), m_dbCurveB( dbCurveB )  {  }

	const C3dCoord& Pt()			{ return m_pt; }

	const CDbCurve* CurveA()		{ return m_dbCurveA; }

	const CDbCurve* CurveB()		{ return m_dbCurveB; }

	virtual ~CIntsctRec()			{  }

protected:

private:  // Methods

private:  // Disabled

	CIntsctRec();
	CIntsctRec( const CIntsctRec& );
	const CIntsctRec& operator = ( const CIntsctRec& );
	int operator == ( const CIntsctRec& ) const;
	int operator != ( const CIntsctRec& ) const;

private:  // Data

	C3dCoord		m_pt;
	const CDbCurve*	m_dbCurveA;
	const CDbCurve*	m_dbCurveB;
};

typedef CIndxList<CIntsctRec*> CIntsctRecList;

#endif

