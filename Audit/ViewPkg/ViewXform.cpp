// ==================================================================
//		ViewXform
//
// ==================================================================

#include "stdafx.h"
#include "ViewXform.h"

// ==================================================================

CViewXform::CViewXform()
	: m_view_extent( -1, -1, 1, 1 )
{
}

CViewXform::CViewXform( const CViewXform& xform )
	: m_view_extent( xform.m_view_extent )
{
}


CViewXform::~CViewXform()
{
}


void	
CViewXform::Scale( double in_factor )
{
	C2dCoord tlpt( m_view_extent.Xmin(), m_view_extent.Ymin() );
	C2dCoord brpt( m_view_extent.Xmax(), m_view_extent.Ymax() );
	C2dCoord ctr( m_view_extent.Xc(), m_view_extent.Yc() );

	C2dVec tlvec = tlpt - ctr;
	C2dVec brvec = brpt - ctr;

	tlvec *= in_factor;
	brvec *= in_factor;

	tlpt = ctr + tlvec;
	brpt = ctr + brvec;

	m_view_extent.Update( tlpt.X(), tlpt.Y(), brpt.X(), brpt.Y() );
}


void	
CViewXform::Center( const C2dCoord& in_ctr )
{
	C2dVec delta(
		in_ctr.X() - m_view_extent.Xc(),
		in_ctr.Y() - m_view_extent.Yc() );
	Pan( delta );
}

void	
CViewXform::Pan( const C2dVec& in_pan )
{
	m_view_extent.Update(
		m_view_extent.Xmin() + in_pan.X(),
		m_view_extent.Ymin() + in_pan.Y(),
		m_view_extent.Xmax() + in_pan.X(),
		m_view_extent.Ymax() + in_pan.Y() );
}

