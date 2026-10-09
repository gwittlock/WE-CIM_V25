// ==================================================================
//		PanelList
//
// ==================================================================

#include "stdafx.h"
#include "PanelList.h"

// ==================================================================

CPanelList::CPanelList()
{
}

CPanelList::~CPanelList()
{
	m_datalist.DestructiveFlush();
}

CPanelList::CPanelList( const CPanelList& panel_list )
{
	*this = panel_list;
}

const CPanelList&
CPanelList::operator =( const CPanelList& panel_list )
{
	m_datalist.DestructiveFlush();
	for (int idx=0; idx<panel_list.Count(); idx++)
		Add( panel_list[idx] );

	return *this;
}

// ==================================================================
//		Add
//
//	Add a panel to the list... minimum parameters necessary to nest
//
CReturn	
CPanelList::Add( 
	const C2dBox&	extent, 
	int				quant, 
	int				pass,
	bool			rotate )
{
	CReturn	ret;

	CPanelData*	new_panel = new CPanelData( extent, quant, pass );
	if (!new_panel)
	{
		ret.Fatal( IDS_INTERNAL_ERROR, "CPanelList:Add, memory allocation failure" );
		return ret;
	}

	new_panel->Rotation( rotate );
	m_datalist.Append( new_panel );

	return ret;
}


