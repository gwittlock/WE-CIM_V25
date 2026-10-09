
#ifndef _WMNODE_H
#define _WMNODE_H

#include "stdafx.h"


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Definition
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// Doubly-linked node having prev/next (or left/right)
// pointers to its adjacent doubly-linked nodes.

class dllExport CWmNode
{
public:

	/////////////////////////////////////////////////////////////////
	// The previous and next nodes will both be NULL.
	CWmNode();

	/////////////////////////////////////////////////////////////////
	// Obtain a pointer to this nodes' previous node.
	CWmNode* Prev() const;

	/////////////////////////////////////////////////////////////////
	// Obtain a pointer to this nodes' next node.
	CWmNode* Next() const;

	/////////////////////////////////////////////////////////////////
	// Set this nodes' previous node to be the given node.
	// The 'current' previous node is returned.
	//
	// Given the linked list A-B-C, B->Prev(D) will yield the linked
	// list A-D-B-C and return A as the 'current' previous node.
	//
	CWmNode* Prev( CWmNode* node );

	/////////////////////////////////////////////////////////////////
	// Set this nodes' next node to be the given node.
	// The 'current' next node is returned.
	//
	// Given the linked list A-B-C, B->Next(D) will yield the linked
	// list A-B-D-C and return C as the 'current' next node.
	//
	CWmNode* Next( CWmNode* node );

	/////////////////////////////////////////////////////////////////
	// This node unlinks itself from its neighbors and mends the gap.
	//
	// Given the linked list A-B-C, B->Unlink() will yield the linked
	// list A-C.  This nodes references to its neighbors will be NULLed.
	//
	virtual void Unlink();

	virtual ~CWmNode();

private:

	// Disabled.
	CWmNode( const CWmNode& );
	const CWmNode operator = ( const CWmNode& );
	int operator == ( const CWmNode& );
	int operator != ( const CWmNode& );

private:

	CWmNode* m_prev;
	CWmNode* m_next;
};

#endif