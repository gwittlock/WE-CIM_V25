#if !defined(_PANELDATA_H)
#define _PANELDATA_H

// ==================================================================
//		PanelData
//
// ==================================================================

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Common.h"
#include "2dBox.h"
#include "Return.h"

// ==================================================================

class dllExport CPanelData
{
public:
	CPanelData( const C2dBox& ext, int quant, int pass );
	virtual ~CPanelData();

	const C2dBox&	Extent( void ) const			{ return m_extent; }
	void			Extent( const C2dBox& ext )		{ m_extent = ext; }

	int				Quantity( void ) const			{ return m_quantity; }
	void			Quantity( int quant )			{ m_quantity = quant; }

	int				Pass( void ) const				{ return m_pass; }
	void			Pass( int pass )				{ m_pass = pass; }

	bool			Rotation( void ) const			{ return m_rotate; }
	void			Rotation( bool rotate )			{ m_rotate = rotate; }

protected:

private:
	// Disabled.
	CPanelData();
	CPanelData( const CPanelData& );
	const CPanelData& operator = ( const CPanelData& );
	int operator == ( const CPanelData& ) const;
	int operator != ( const CPanelData& ) const;

private:
	C2dBox	m_extent;
	int		m_quantity;
	int		m_pass;
	bool	m_rotate;
};

#endif

