
#include "stdafx.h"
#include "PartInstance.h"


CPartInstance::CPartInstance(
		const CNestingPart*	part,
		const C2dCoord&		location,
		int					hit )

	: m_part( part ),
	  m_location( location ),
	  m_hit( hit )
{
}

CPartInstance::~CPartInstance()
{
}
