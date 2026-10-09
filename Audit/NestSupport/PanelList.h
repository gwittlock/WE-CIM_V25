#if !defined(_PANELLIST_H)
#define _PANELLIST_H

// ==================================================================
//		PanelList
//
// ==================================================================

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "common.h"

#include "Return.h"
#include "2dBox.h"
#include "IndxList.h"
#include "PanelData.h"

// ==================================================================

class dllExport CPanelList
{
public:
	CPanelList();
	CPanelList( const CPanelList& );

	virtual ~CPanelList();

	const CPanelList& operator =( const CPanelList& );

	int		Count( void ) const									{ return m_datalist.Count(); }

	const CPanelData&	operator[]( int idx ) const				{ return *(m_datalist[idx]); }
	CPanelData*			pPanelData( int idx )					{ return m_datalist[idx]; }

	CReturn	Add( const CPanelData& data )						{ return Add( data.Extent(), data.Quantity(), data.Pass(), data.Rotation() ); }
	CReturn	Add( const C2dBox& extent, int quant, int pass, bool rotate );

protected:

private:
	// Disabled.
	int operator == ( const CPanelList& ) const;
	int operator != ( const CPanelList& ) const;

private:
	CIndxList<CPanelData*>	m_datalist;		// List of parts that may be nested
};

#endif

