
#ifndef _WMELEM_H
#define _WMELEM_H

#ifndef _GEOELEM_H
#include "GeoElem.h"
#endif

#ifndef _WMNODE_H
#include "WmNode.h"
#endif

#ifndef _WMSUBCHN_H
#include "WmSubchn.h"
#endif


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class dllExport CWmElem : public CWmNode
{
public:

	CWmElem( CWmSubchn* owner, CGeoElem* elem );

	CGeoElem* Elem() const				{ return m_elem; }
	void Elem( CGeoElem* elem )
	{
		m_elem = elem;
	}

	/////////////////////////////////////////////////////////////////
	// Get the subchain that contains this element.
	CWmSubchn* Owner() const			{ return m_owner; }
	void Owner( CWmSubchn* owner )		{ m_owner = owner; }

	virtual ~CWmElem();

	virtual void Unlink();

private:  // Disabled.

	CWmElem();
	CWmElem( const CWmElem& node );
	const CWmElem& operator = ( const CWmElem& node );
	int operator == ( const CWmElem& node ) const;
	int operator != ( const CWmElem& node ) const;

private:  // Methods

private:  // Data

	CWmSubchn* m_owner;
	CGeoElem* m_elem;
};

#endif
