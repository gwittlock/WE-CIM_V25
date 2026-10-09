
#ifndef _WORKPKG_H
#define _WORKPKG_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef _2DBOX_H
#include "2dBox.h"
#endif

#ifndef _DYNAMICARRAY_H
#include "DynamicArray.h"
#endif

#ifndef _DBENTITY_H
#include "DbEntity.h"
#endif

#include "3dCoord.h"

class CDbFeature;


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class dllExport CWorkPkg
{
public:

	// NOTE: A topLevelFeature of NULL is used to indicate the 'default' work zone.
	CWorkPkg(
			CDbFeature*	topLevelFeature,
			bool		isSubdef,
			double		xmin,
			double		ymin,
			double		xmax,
			double		ymax );

	const C2dBox&	Box()			{ return m_box; }

	// Introduced (V16) to support the Portal command CodeGen:Optimize:
	void Box( const C2dBox& box )	{ m_box = box; }

	CDbFeature*		Feature()		{ return m_feature; }

	bool			IsSubdef()		{ return m_isSubdef; }

	CDbEntityArray&	Entities()		{ return m_entities; }

	const CVarList&	Attrib()		{ return m_attribs; }
	CVarList*		pAttrib()		{ return &m_attribs; }

	C3dCoord		StartPt();

	void			BoxUpdate();

	virtual ~CWorkPkg();

protected:

private:  // Methods

private:  // Disabled

	CWorkPkg();
	CWorkPkg( const CWorkPkg& );
	const CWorkPkg& operator = ( const CWorkPkg& );
	int operator == ( const CWorkPkg& ) const;
	int operator != ( const CWorkPkg& ) const;

private:  // Data

	CDbFeature*		m_feature;
	C2dBox			m_box;
	CDbEntityArray	m_entities;
	CVarList		m_attribs;
	bool			m_isSubdef;
};

typedef CDynamicArray<CWorkPkg*> CWorkPkgArray;

#endif

