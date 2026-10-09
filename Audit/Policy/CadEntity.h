#ifndef _CADENTITY_H
#define _CADENTITY_H

#include "DynamicArray.h"
#include "3dVec.h"
#include "GeoElem.h"


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class dllExport CCadEntity
{
public:

	CCadEntity(
			long			handle,
			const CString&	layerName,
			const C3dVec&	planeNormal,
			CGeoElem*		entity )
	{
		m_handle	= handle;
		m_layer		= layerName;
		m_normal	= planeNormal;
		m_entity	= entity;
	}

	long			Handle()		{ return m_handle; }

	const CString&	LayerName()		{ return m_layer; }

	const C3dVec&	Normal()		{ return m_normal; }

	const CGeoElem*	Entity()		{ return m_entity; }

	~CCadEntity()					{ delete m_entity; }

private:

	// Disabled.
	CCadEntity();

private:

	long		m_handle;
	CString		m_layer;
	C3dVec		m_normal;
	CGeoElem*	m_entity;
};

typedef CDynamicArray<CCadEntity*> CCadEntityList;


#endif