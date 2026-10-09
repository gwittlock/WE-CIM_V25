
#ifndef _WMCRVRECEXTDATA_H
#define _WMCRVRECEXTDATA_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef _INDXLIST_H
#include "IndxList.h"
#endif

#ifndef _2DCOORD_H
#include "2dCoord.h"
#endif

class CWmCrvRec;
class CWmIntRec;
class CWmSubchn;

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class CWmCrvRecExtData
{
public:

	CWmCrvRecExtData( const CWmCrvRec* owner, const CWmIntRec* intRec, double uparam );

	const CWmCrvRec* Owner() const		{ return m_owner; }

	double Uparam() const				{ return m_uparam; }

	C2dCoord Point() const;

	const CWmCrvRec* WmCrvRec() const	{ return m_owner; }

	const CWmIntRec* IntRec() const		{ return m_intrec; }

	CWmSubchn* Subchn() const			{ return m_subchn; }

	void Subchn( CWmSubchn* subchn );

	void Tag()							{ m_tagged = TRUE; }

	bool IsTagged() const				{ return m_tagged; }

	virtual ~CWmCrvRecExtData();

protected:

private:  // Methods

private:  // Disabled

	CWmCrvRecExtData();
	CWmCrvRecExtData( const CWmCrvRecExtData& );
	const CWmCrvRecExtData& operator = ( const CWmCrvRecExtData& );
	int operator == ( const CWmCrvRecExtData& ) const;
	int operator != ( const CWmCrvRecExtData& ) const;

private:  // Data

	const CWmCrvRec*	m_owner;
	const CWmIntRec*	m_intrec;
	double				m_uparam;
	CWmSubchn*			m_subchn;
	bool				m_tagged;
};

typedef CIndxList<CWmCrvRecExtData*> CWmCrvRecExtDataList;

#endif

