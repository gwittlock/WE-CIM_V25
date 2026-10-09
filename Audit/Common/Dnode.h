
#ifndef _DNODE_H
#define _DNODE_H

#include "stdafx.h"

#ifndef _AFXTEMPL_H
#include <afxtempl.h>
#define _AFXTEMPL_H
#endif


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Definition
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// Doubly-linked node having prev/next (or left/right)
// pointers to its adjacent doubly-linked nodes.

template <class T>
class dllExport CDnode
{
public:

	/////////////////////////////////////////////////////////////////
	// The previous and next nodes will both be NULL.
	CDnode( T item );

	/////////////////////////////////////////////////////////////////
	// Obtain this nodes' data item.
	T Item() const;

	/////////////////////////////////////////////////////////////////
	// Set this nodes' data item.
	void Item( T item );

	/////////////////////////////////////////////////////////////////
	// Remove the data from this node in a benign manner.
	void Flush();

	/////////////////////////////////////////////////////////////////
	// Obtain a pointer to this nodes' previous node.
	CDnode<T>* Prev() const;

	/////////////////////////////////////////////////////////////////
	// Obtain a pointer to this nodes' next node.
	CDnode<T>* Next() const;

	/////////////////////////////////////////////////////////////////
	// Set this nodes' previous node to be the given node.
	// The 'current' previous node is returned.
	//
	// Given the linked list A-B-C, B->Prev(D) will yield the linked
	// list A-D-B-C and return A as the 'current' previous node.
	//
	CDnode<T>* Prev( CDnode* node );

	/////////////////////////////////////////////////////////////////
	// Set this nodes' next node to be the given node.
	// The 'current' next node is returned.
	//
	// Given the linked list A-B-C, B->Next(D) will yield the linked
	// list A-B-D-C and return C as the 'current' next node.
	//
	CDnode<T>* Next( CDnode* node );

	/////////////////////////////////////////////////////////////////
	// This node unlinks itself from its neighbors and mends the gap.
	//
	// Given the linked list A-B-C, B->Unlink() will yield the linked
	// list A-C.  This nodes references to its neighbors will be NULLed.
	//
	void Unlink();

	~CDnode();

private:

	// Disabled.
	CDnode( const CDnode& );
	const CDnode operator = ( const CDnode& );
	int operator == ( const CDnode& );
	int operator != ( const CDnode& );

private:

	CDnode* m_prev;
	CDnode* m_next;

	T m_item;
};



//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Implementation.
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

template <class T>
CDnode<T>::CDnode( T item )

	: m_prev( NULL ),
	  m_next( NULL ),
	  m_item( item )
{
}

template <class T>
CDnode<T>::~CDnode()
{
	ASSERT( (m_prev == NULL && m_next == NULL) );
	delete m_item;
}

template <class T>
AFX_INLINE T
CDnode<T>::Item() const
{
	return m_item;
}

template <class T>
AFX_INLINE void
CDnode<T>::Item( T item )
{
	m_item = item;
}

template <class T>
AFX_INLINE void
CDnode<T>::Flush()
{
	m_item = NULL;
}

template <class T>
AFX_INLINE CDnode<T>*
CDnode<T>::Prev() const
{
	return m_prev;
}

template <class T>
AFX_INLINE CDnode<T>*
CDnode<T>::Next() const
{
	return m_next;
}

template <class T>
AFX_INLINE CDnode<T>*
CDnode<T>::Prev( CDnode* node )
{
	node->m_prev = m_prev;
	node->m_next = this;

	if (m_prev != NULL)
		m_prev->m_next = node;

	m_prev = node;

	return node->m_prev;
}

template <class T>
AFX_INLINE CDnode<T>*
CDnode<T>::Next( CDnode* node )
{
	node->m_prev = this;
	node->m_next = m_next;

	if (m_next != NULL)
		m_next->m_prev = node;

	m_next = node;

	return node->m_next;
}

template <class T>
AFX_INLINE void
CDnode<T>::Unlink()
{
	if (m_prev != NULL)
		m_prev->m_next = m_next;

	if (m_next != NULL)
		m_next->m_prev = m_prev;

	m_prev = NULL;
	m_next = NULL;
}

#endif