#if !defined(_NESTROTATION_H)
#define _NESTROTATION_H
#pragma once

// ==================================================================
//		NestRotation
//
// ==================================================================

#include "Common.h"
#include "Model.h"

#include "NestConfig.h"
#include "DexGrid.h"
#include "DbCurve.h"

#include "DynamicArray.h"


// ==================================================================



class dllExport CNestRotation
{
public:
	CNestRotation();
	~CNestRotation();

	CModel*		Model( void ) const		{ return m_model; }
	void		Model( CModel* model );

	CModel*		Outline( void ) const	{ return m_outline; }
	void		Outline( CModel* outline );

	CDexGrid*	DexGrid( void ) const	{ return m_dexgrid; }
	void		DexGrid( CDexGrid* grid );

private:
	CModel*		m_model;		// Model holding original geometry; NestRotation OWNS this
	CModel*		m_outline;		// Model holding a simplified outline; NestRotation OWNS this
	CDexGrid*	m_dexgrid;		// Nesting grid array; NestPart OWNS this
};

typedef CDynamicArray<CNestRotation*> CNestRotationArray;

#endif
