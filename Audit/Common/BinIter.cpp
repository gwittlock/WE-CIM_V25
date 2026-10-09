#include "MathConst.h"
#include "assert.h"
#include "BinIter.h"


CBinIter::CBinIter()
{
	m_max = IUNDEFINED;
	m_min = IUNDEFINED;
	m_mid = IUNDEFINED;
}

CBinIter::~CBinIter()
{
}

void
CBinIter::IterInit( int min, int max )
{
	assert( (max > min) );
	m_min = min;
	m_max = max;
	m_mid = IUNDEFINED;
}

int
CBinIter::Next( int dir )
{
	int next = IUNDEFINED;

	assert( (dir != 0) );

	if (m_max < m_min)
	{
		// No iterations remaining.
	}
	else
	{
		if (m_mid < IUNDEFINED)
		{
			if (dir > 0)
				m_min = m_mid + 1;
			else
				m_max = m_mid - 1;
		}
		else
		{
			// Nothing to do. This is the first call.
		}

		m_mid = (m_max + m_min) / 2;
		next = m_mid;
	}

	return next;
}

int
CBinIter::Mid() const
{
	return m_mid;
}
