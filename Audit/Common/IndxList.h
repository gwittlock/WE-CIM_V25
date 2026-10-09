
#ifndef _INDXLIST_H
#define _INDXLIST_H

#include "stdafx.h"

#ifndef _AFXTEMPL_H
#include <afxtempl.h>
#define _AFXTEMPL_H
#endif

#ifndef _DNODE_H
#include "Dnode.h"
#endif

// NOTE: Perhaps this should be modernized using STL.

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Definition
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

template <class T>
class dllExport CIndxList
{
public:

	CIndxList();

	///////////////////////////////////////////////////////////////
	// Executes BenignFlush() before calling the base destructor
	// because, by default, the list acts as if it does not own
	// the data that it contains.  If you want to destroy the
	// contained data, then call DestructiveFlush().
	//
	~CIndxList();

	///////////////////////////////////////////////////////////////
	// Obtain the number of items in the list.
	//
	int Count() const;

	///////////////////////////////////////////////////////////////
	// Our current position in the list.
	//
	int Curr() const;

	///////////////////////////////////////////////////////////////
	// Access the ith item in the list.
	//
	T operator [] ( int indx ) const;
	T GetAt( int indx ) const;

	///////////////////////////////////////////////////////////////
	// Obtain the list index of the given item.
	// Returns negative number when not found.
	//
	int Find( const T item ) const;

	///////////////////////////////////////////////////////////////
	// Add an item to the beginning of the list.
	//
	int Prepend( T item );

	///////////////////////////////////////////////////////////////
	// Add an item to the end of the list.
	//
	int Append( T item );

	///////////////////////////////////////////////////////////////
	// If the item is not in the list than add an item to the end.
	//
	int ConditionalAppend( T item );

	///////////////////////////////////////////////////////////////
	// Insert an item before the ith item in the list.
	//
	int InsertBefore( int indx, T item );

	///////////////////////////////////////////////////////////////
	// Insert an item after the ith item in the list.
	//
	int InsertAfter( int indx, T item );

	///////////////////////////////////////////////////////////////
	// Replace the ith item in the list with a different item.
	//
	T Replace( int indx, T item );

	///////////////////////////////////////////////////////////////
	// Remove the ith item in the list.
	//
	//   If you are removing the current item, that is,
	//   the item last accessed using the [] operator,
	//   your new index into the list will be one of:
	//
	//   1. If the item is the last item in the list,
	//      your new index will be (indx - 1).
	//
	//   2. Otherwise, the index will be unchanged but data
	//      will be (what was formerly) the (indx +1 ) item.
	//
	//   If you are removing an item preceding the current
	//   item, your new index will be (indx - 1).
	//
	T Remove( int indx );

	///////////////////////////////////////////////////////////////
	// Remove all items from the list.
	//
	//   This is non-destructive.  You must manage destruction
	//   of your data that is in the list.
	//
	void BenignFlush();

	///////////////////////////////////////////////////////////////
	// Remove all items from the list.
	//
	//   This is destructive, ie. the list owns the data.
	//
	void DestructiveFlush();

	// For low-level debugging.
	// Historically, encountered crashing during debugging of
	// spiral pocketing with islands.  Somehow a list object
	// had a count==33 && m_head==NULL
	CDnode<T>* Head() const;
	CDnode<T>* Tail() const;

private:

	// Disabled.
	CIndxList( const T& );
	const T& operator = ( const T& );

private:  // Methods
	
	void Init();
	CDnode<T>* MoveTo( int indx );

private:  // Data

	CDnode<T>* m_head;
	CDnode<T>* m_tail;

	int m_count;

	int m_indx;
	CDnode<T>* m_node;
};



//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Implementation.
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

template <class T>
CIndxList<T>::CIndxList()
{
	Init();
}

template <class T>
CIndxList<T>::~CIndxList()
{
	BenignFlush();
}

template <class T>
AFX_INLINE int
CIndxList<T>::Count() const
{
	return m_count;
}

template <class T>
AFX_INLINE int
CIndxList<T>::Curr() const
{
	return m_indx;
}

template <class T>
AFX_INLINE T
CIndxList<T>::operator [] ( int indx ) const
{
	CDnode<T>* node = ((CIndxList<T>*) this)->MoveTo( indx );
	return m_node->Item();
}

template <class T>
AFX_INLINE T
CIndxList<T>::GetAt( int indx ) const
{
	CDnode<T>* node = ((CIndxList<T>*) this)->MoveTo( indx );
	return m_node->Item();
}

template <class T>
int 
CIndxList<T>::Find( const T item ) const
{
	for (int indx = 0; indx < m_count; ++indx)
	{
		if ((*this)[indx] == item)
			return indx;
	}

	return -1;
}

template <class T>
AFX_INLINE int
CIndxList<T>::Prepend( T item )
{
	CDnode<T>* newNode = new CDnode<T>( item );

	ASSERT( (newNode != NULL) );

	if (m_head == NULL)
	{
		m_head = newNode;
		m_tail = m_head;
	}
	else
	{
		m_head->Prev( newNode );
		m_head = newNode;
	}

	++m_count;

	if (m_indx >= 0)
	{
		// Must be careful to maintain our position.
		m_indx = ((m_count == 1) ? 0 : (m_indx+1));
	}

	return m_count;
}

template <class T>
AFX_INLINE int
CIndxList<T>::Append( T item )
{
	CDnode<T>* newNode = new CDnode<T>( item );

	ASSERT( (newNode != NULL) );

	if (m_head == NULL)
	{
		m_head = newNode;
		m_tail = m_head;
	}
	else
	{
		m_tail->Next( newNode );
		m_tail = newNode;
	}

	++m_count;

	return m_count;
}

template <class T>
AFX_INLINE int
CIndxList<T>::ConditionalAppend( T item )
{
	int status = Find( item );

	if (status < 0)
		status = Append( item );

	return status;
}


template <class T>
int
CIndxList<T>::InsertBefore( int indx, T item )
{
	if (m_count == 0 || indx == m_count)
	{
		Append( item );
	}
	else
	{
		CDnode<T>* newNode = new CDnode<T>( item );

		ASSERT( (newNode != NULL) );

		// Remember our current position in the list.
		int indxSave = m_indx;
		CDnode<T>* nodeSave = m_node;

		// Move to the new position and insert the item.
		CDnode<T>* refNode = MoveTo( indx );

		// And insert the item.
		refNode->Prev( newNode );

		// Restore our original position.
		m_indx = ((indx > indxSave) ? indxSave : (indxSave+1));
		m_node = nodeSave;

		++m_count;

		if (refNode == m_head)
			m_head = newNode;
	}

	return m_count;
}

template <class T>
int
CIndxList<T>::InsertAfter( int indx, T item )
{
	CDnode<T>* newNode = new CDnode<T>( item );

	ASSERT( (newNode != NULL) );

	// Remember our current position in the list.
	int indxSave = m_indx;
	CDnode<T>* nodeSave = m_node;

	// Move to the new position.
	CDnode<T>* refNode = MoveTo( indx );

	// And insert the item.
	refNode->Next( newNode );

	// Restore our original position.
	m_indx = ((indx >= indxSave) ? indxSave : (indxSave+1));
	m_node = nodeSave;

	++m_count;

	if (refNode == m_tail)
		m_tail = newNode;

	return m_count;
}

template <class T>
T
CIndxList<T>::Replace( int indx, T item )
{
	// Remember our current position in the list.
	int indxSave = m_indx;
	CDnode<T>* nodeSave = m_node;

	// Move to the new position.
	CDnode<T>* refNode = MoveTo( indx );

	T prevItem = refNode->Item();

	// And replace the item.
	refNode->Item( item );

	// Restore our original position.
	m_indx = indxSave;
	m_node = nodeSave;

	return prevItem;
}

template <class T>
T
CIndxList<T>::Remove( int indx )
{
	// Remember our current position in the list.
	int indxSave = m_indx;
	CDnode<T>* nodeSave = m_node;

	// Move to the new position.
	CDnode<T>* refNode = MoveTo( indx );

	// Restore our original position.
	m_indx = indxSave;
	m_node = nodeSave;

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// We must take care to properly adjust our position
	// to account for the removal of an item.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	if (refNode == m_head)
	{
		// We're removing the head of the list.

		if (m_node == m_head)
		{
			m_node = m_head->Next();
			m_indx = 0;
		}
		else
		{
			--m_indx;
		}

		m_head = m_head->Next();

		if (m_head == NULL)
		{
			// We now have an empty list;

			m_tail = NULL;
			m_indx = -1;
		}
	}
	else if (refNode == m_tail)
	{
		// We're removing the tail end of the list.

		if (m_node == m_tail)
		{
			m_node = m_tail->Prev();
			--m_indx;
		}

		m_tail = m_tail->Prev();
	}
	else if (refNode == m_node)
	{
		// The next item simply becomes the current item,
		// and the current index remains unaffected.

		m_node = m_node->Next();
	}
	else if (indx < m_indx)
	{
		--m_indx;
	}
	else
	{
		// The node being removed is beyond our current position
		// and thus has no effect on our current position.
	}


	T theData = refNode->Item();
	refNode->Flush();

	refNode->Unlink();
	delete refNode;

	--m_count;

	return theData;
}

template <class T>
void
CIndxList<T>::BenignFlush()
{
	CDnode<T>* next = NULL;
	CDnode<T>* curr = m_head;

	while (curr != NULL)
	{
		curr->Flush();

		next = curr->Next();

		curr->Unlink();
		delete curr;

		curr = next;
	}

	Init();
}

template <class T>
void
CIndxList<T>::DestructiveFlush()
{
	CDnode<T>* next = NULL;
	CDnode<T>* curr = m_head;

	while (curr != NULL)
	{
		next = curr->Next();

		curr->Unlink();
		delete curr;

		curr = next;
	}

	Init();
}

template <class T>
CDnode<T>*
CIndxList<T>::MoveTo( int indx )
{
	if (indx == m_indx)
		return m_node;  // Nothing to do because we're already there.

	// TODO: assert() if (indx < 0 || indx >= Count()) ?
	// TODO:  If m_indx<0 and indx != 0 or n-1, this FAILS!
	// PATCH:  If we are m_indx -1, just force the bastard to zero.
	if (m_indx < 0)
	{
		m_indx = 0;
		m_node = m_head;
	}

	// Simplest case is jumping to either end of list.
	if (indx == 0)
	{
		m_node = m_head;
		m_indx = 0;
		return m_node;
	}

	if (indx == (m_count-1))
	{
		m_node = m_tail;
		m_indx = m_count - 1;
		return m_node;
	}


	// Next simplest case is move to adjacent index.
	int delta = indx - m_indx;

	if (delta == 1)
	{
		m_node = m_node->Next();
		m_indx = indx;
		return m_node;
	}

	if (delta == -1)
	{
		m_node = m_node->Prev();
		m_indx = indx;
		return m_node;
	}


	// The most complex cases require computing shortest traversal.
	if (delta < 0)
		delta = -delta;

	int forwards;

	if (m_indx < indx)
	{
		int otherDelta = m_count - indx;

		forwards = (otherDelta >= delta);

		if ( !forwards )
		{
			// Prepare for a backwards traversal.
			m_indx = m_count - 1;
			m_node = m_tail;
		}
	}
	else
	{
		forwards = (indx < delta);

		if ( forwards )
		{
			// Prepare for a forwards traversal.
			m_indx = 0;
			m_node = m_head;
		}
	}

	while (m_indx != indx)
	{
		if ( forwards )
		{
			m_node = m_node->Next();
			++m_indx;
		}
		else
		{
			m_node = m_node->Prev();
			--m_indx;
		}
	}

	return m_node;
}

template <class T>
CDnode<T>*
CIndxList<T>::Head() const
{
	return m_head;
}

template <class T>
CDnode<T>*
CIndxList<T>::Tail() const
{
	return m_tail;
}

template <class T>
AFX_INLINE void
CIndxList<T>::Init()
{
	m_head  = NULL;
	m_tail  = NULL;
	m_count = 0;
	m_indx  = -1;
	m_node  = NULL;
}


#endif