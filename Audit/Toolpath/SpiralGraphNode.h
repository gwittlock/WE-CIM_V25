
#ifndef _SPIRALGRAPHNODE_H
#define _SPIRALGRAPHNODE_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "DynamicArray.h"
#include "3dCoord.h"
#include "WmChain.h"

typedef struct
{
	C3dCoord	pt_parent;
	CWmElem*	wmelem_parent;
	C3dCoord	pt_child;
	CWmElem*	wmelem_child;
	bool		is_valid;
} tClosestPointPair;

class CSpiralGraphNode;
typedef CDynamicArray<CSpiralGraphNode*>	tSGNArray;
typedef CDynamicArray<CWmElem*>	tWmElemArray;

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class dllExport CSpiralGraphNode
{
public:

	CSpiralGraphNode( CWmChain* chain );

	int	ChildCount();

	void ChildAdd( CSpiralGraphNode* child );

	CSpiralGraphNode* ChildGet( int indx );

	void Split( double dist );

	void Reseed( CWmElem* wmelem );

	// For debugging.
	void Dump( int level );

	virtual ~CSpiralGraphNode();

protected:

private:  // Disabled

	CSpiralGraphNode( const CSpiralGraphNode& );
	const CSpiralGraphNode& operator = ( const CSpiralGraphNode& );
	int operator == ( const CSpiralGraphNode& ) const;
	int operator != ( const CSpiralGraphNode& ) const;

private:  // Methods

	tClosestPointPair Split(
						const CWmChain&	parent,
						const CWmChain&	child,
						double			dist );

	double ClosestPoints(
					const CGeoCurve&	curveA,
					const CGeoCurve&	curveB,
					C3dCoord*			ptA,
					C3dCoord*			ptB );

	void Split(
				CWmElem*		from_wmelem,
				const C3dCoord&	from_pt,
				CWmElem*		to_wmelem,
				const C3dCoord&	to_pt );

private:  // Data

	CWmChain*		m_chain;
	tSGNArray		m_children;
	C3dCoordArray	m_pts;
	tWmElemArray	m_wmelems;
};

#endif

