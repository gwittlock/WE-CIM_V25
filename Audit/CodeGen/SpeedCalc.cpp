
#include "stdafx.h"
#include <math.h>
#include "StringConst.h"

#include "SpeedCalc.h"

// =======================================================================
CSpeedCalc::CSpeedCalc()
	: m_feedrates(),
	  m_okay( FALSE )
{
}

// Per conversation with Tom & Gary (2001/02/20), the Slowdown
// Formula is to be applied as a percentage of the current feedrate,
// as in 'slowdown by 30%'
bool CSpeedCalc::Init( const CVarList& sfparams )
{
	m_feedrates.SetSize( 10, 10 );

	m_ndivs = sfparams.getInt( "Slowdown_Divisions", 0 );

	m_setback = fabs( sfparams.getReal( "Slowdown_Setback", 0. ) );
	m_factor  = fabs( sfparams.getReal( "Slowdown_Formula", 0. ) );
	m_maxfeed = fabs( sfparams.getReal( STR_FEED, 0. ) );

	m_feedrates[0] = m_maxfeed;
	
	m_okay = (m_ndivs >0 &&
			  m_setback > 0 &&
			  m_factor > 0 &&
			  m_maxfeed > 0);

	if ( m_okay )
	{
		for (int indx = 1; indx <= m_ndivs; ++indx)
		{
			double next = m_feedrates[indx-1] * ( 1.0 - m_factor);

			m_feedrates[indx] = ((next > 0.0) ? next : 0.0);
		}
	}

	return m_okay;
}

CSpeedCalc::~CSpeedCalc()
{
}

double
CSpeedCalc::Feed( int interval_indx ) const
{
	return (m_feedrates[m_ndivs - interval_indx]);
}
