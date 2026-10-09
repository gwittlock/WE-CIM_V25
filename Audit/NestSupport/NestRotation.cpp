// ==================================================================
//		NestRotation
//
// ==================================================================

#include "stdafx.h"
#include "Common.h"
#include "NestRotation.h"

// ==================================================================

CNestRotation::CNestRotation()
	: m_model(NULL),
	  m_outline(NULL),
	  m_dexgrid(NULL)
{
}

CNestRotation::~CNestRotation()
{
	if (m_model) { delete m_model; m_model = NULL; }
	if (m_outline) { delete m_outline; m_outline = NULL; }
	if (m_dexgrid) { delete m_dexgrid; m_dexgrid = NULL; }
}

// ==================================================================

void		
CNestRotation::Model( CModel* model )
{ 
	if (m_model) delete m_model;
	m_model = model;
}


void		
CNestRotation::Outline( CModel* model )
{ 
	if (m_outline) delete m_outline;
	m_outline = model;
}


void
CNestRotation::DexGrid( CDexGrid* grid )
{
	if (m_dexgrid) delete m_dexgrid;
	m_dexgrid = grid;
}