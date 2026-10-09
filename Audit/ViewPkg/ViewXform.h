#if !defined(_VIEWXFORM_H)
#define _VIEWXFORM
#pragma once

#include <math.h>
#include "2dBox.h"

// ==================================================================

class CViewXform
{
public:

	CViewXform();

	CViewXform( const CViewXform& xform );

	virtual ~CViewXform();

	const C2dBox& ViewExtent() const
		{ return m_view_extent; }

	void ViewExtent( const C2dBox& extent )
		{ m_view_extent = extent; }

	double ViewScale() const
		{ return m_view_scale; }

	void ViewScale( double scale )
		{ m_view_scale = scale; }

	// delta Transformations...
	void Scale( double in_factor );
	void Center( const C2dCoord& in_ctr );
	void Pan( const C2dVec& in_pan );

private:
	// Disabled.
	const CViewXform& operator = ( const CViewXform& );
	int operator == ( const CViewXform& ) const;
	int operator != ( const CViewXform& ) const;

private:

	// The extents of the model that are displayed.
	C2dBox	m_view_extent;
	double	m_view_scale;

	// Rotation angle
	//   double	m_angle;
};

#endif

