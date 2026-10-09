#ifndef _PARTINSTANCE_H
#define _PARTINSTANCE_H

#include "2dCoord.h"
#include "NestingPart.h"

// Introduced to support a new "seeding algorithm".  Allows an instance
// of CSheet to track the part instances that are placed on it.

class dllExport CPartInstance
{
public:

	CPartInstance(
		const CNestingPart*	part,
		const C2dCoord&		location,
		int					hit );
	
	// Not virtual because we won't ever have subclasses.
	~CPartInstance();

	const CNestingPart* Part() const		{ return m_part; }

	const C2dCoord& Location() const		{ return m_location; }

	int Hit() const							{ return m_hit; }

private:

	// Disabled.
	CPartInstance();
	CPartInstance( const CPartInstance& );
	const CPartInstance& operator = ( const CPartInstance& );
	int operator == ( const CPartInstance& ) const;
	int operator != ( const CPartInstance& ) const;

private:

	const CNestingPart*	m_part;		// the part (like duh!)
	C2dCoord			m_location;	// handle point
	int					m_hit;		// orientation ordinal
};

typedef CDynamicArray<CPartInstance*> tPartInstanceArray;

#endif
