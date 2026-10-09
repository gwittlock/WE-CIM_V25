// ==================================================================
//		PanelData
//
// ==================================================================

#include "stdafx.h"
#include "PanelData.h"

// ==================================================================

CPanelData::CPanelData(
	const C2dBox& ext, 
	int quant, 
	int pass )
	: m_extent( ext),
	m_quantity( quant ),
	m_pass( pass )
{
}

CPanelData::~CPanelData()
{
}


// ==================================================================
