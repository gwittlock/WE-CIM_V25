
#ifndef _DYNAMICARRAY_H
#define _DYNAMICARRAY_H

#include "stdafx.h"

#ifndef _AFXTEMPL_H
#include <afxtempl.h>
#define _AFXTEMPL_H
#endif

#include "cmn_resource.h"
#include "MathConst.h"
#include "Return.h"

#define DEFAULT_REALLOC_INCREMENT 10


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Intended as a dynamic array of pointers, replaces use of
// MFC CArray (or CPtrArray) which appear to cause memory leaks.
//
// NOTE: An object of this type does not own its contained data.
//       See also ~CDynamicArray(), BenignFlush(), DestructiveFlush()
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

template <class T>
class dllExport CDynamicArray
{
public:

	///////////////////////////////////////////////////////////////
	// Sets the default reallocation increment to 10 items.
	//
	CDynamicArray();

	///////////////////////////////////////////////////////////////
	// Executes a BenignFlush() before calling the base destructor
	// because, by default, the array acts as if it does not own
	// the data that it contains.  If you want to destroy the
	// contained data, then call DestructiveFlush().
	//
	~CDynamicArray();

	///////////////////////////////////////////////////////////////
	// Sets the reallocation increment.
	//
	void ReallocationIncrement( int delta );

	///////////////////////////////////////////////////////////////
	// Obtain the number of items in the array.
	//
	int Count() const;

	///////////////////////////////////////////////////////////////
	// Access the ith item in the array.
	//
	T operator [] ( int indx ) const;

	///////////////////////////////////////////////////////////////
	// Obtain the array index of the given item.
	// Returns negative number when not found.
	//
	int Find( const T item ) const;

	///////////////////////////////////////////////////////////////
	// Find the index of an item using a comparitor.
	// Assumes an increasing order sorted array.
	//
	// If item is found
	//    returns TRUE and indx represents item position
	// else
	//    returns FALSE and indx represents lo position
	//
	// The comparitor must return:
	//		(a negative value) when arrayItem has lower order myItem
	//		      (zero)       when arrayItem match
	//		(a positive value) when arrayItem has higher order myItem
	//
	bool BinarySearch(
					void*	myItem,
					int		(*func)( void* myItem, void* arrayItem ),
					int*	indx ) const;

	///////////////////////////////////////////////////////////////
	// Add an item to the beginning of the array.
	//
	int Prepend( T item );

	///////////////////////////////////////////////////////////////
	// Add an item to the end of the array.
	//
	int Append( T item );

	///////////////////////////////////////////////////////////////
	// If the item is not in the array than add an item to the end.
	//
	int ConditionalAppend( T item );

	///////////////////////////////////////////////////////////////
	// Insert an item before the ith item in the array.
	//
	int InsertBefore( int indx, T item );

	///////////////////////////////////////////////////////////////
	// Insert an item after the ith item in the array.
	//
	int InsertAfter( int indx, T item );

	///////////////////////////////////////////////////////////////
	// Replace the ith item in the array with a different item.
	//
	T Replace( int indx, T item );

	///////////////////////////////////////////////////////////////
	// Remove the ith item in the array.
	//
	//   If you are removing the current item, that is,
	//   the item last accessed using the [] operator,
	//   your new index into the array will be one of:
	//
	//   1. If the item is the last item in the array,
	//      your new index will be (indx - 1).
	//
	//   2. Otherwise, the index will be unchanged but data
	//      will be (what was formerly) the (indx +1 ) item.
	//
	//   If you are removing an item preceding the current
	//   item, your new index will be (indx - 1).
	//
	T Remove( int indx );

	bool Qsort( int (__cdecl *compare)(const void *elem1, const void *elem2 ) );

	///////////////////////////////////////////////////////////////
	// Benignly remove all items from the array.
	//
	//   This is non-destructive.  You must manage destruction
	//   of your data that is in the array.
	//
	void BenignFlush();

	///////////////////////////////////////////////////////////////
	// Remove and destroys all items in the array.
	//
	//   This is destructive, ie. the array owns the data.
	//
	void DestructiveFlush();

private:

	// Disabled.
	CDynamicArray( const T& );
	const T& operator = ( const T& );

private:  // Methods
	
	void Grow( int delta );

private:  // Data

	T* m_array;

	int m_size;
	int m_delta;
	int m_count;
};



//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Implementation.
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

template <class T>
CDynamicArray<T>::CDynamicArray()
	: m_array( NULL ),
	  m_size( 0 ),
	  m_delta( DEFAULT_REALLOC_INCREMENT ),
	  m_count( 0 )
{
	Grow( m_delta );
}

template <class T>
CDynamicArray<T>::~CDynamicArray()
{
	BenignFlush();
	delete [] m_array;
}

template <class T>
AFX_INLINE void
CDynamicArray<T>::ReallocationIncrement( int delta )
{
	m_delta = delta;
}

template <class T>
AFX_INLINE int
CDynamicArray<T>::Count() const
{
	return m_count;
}

template <class T>
AFX_INLINE T
CDynamicArray<T>::operator [] ( int indx ) const
{
	return m_array[ indx ];
}

template <class T>
int 
CDynamicArray<T>::Find( const T item ) const
{
	for (int indx = 0; indx < m_count; ++indx)
	{
		if (m_array[indx] == item)
			return indx;
	}

	return -1;
}

template <class T>
AFX_INLINE int
CDynamicArray<T>::Prepend( T item )
{
	if ((m_count+1) >= m_size)
		Grow( m_delta );

	for (int indx = m_count; indx > 0; --indx)
	{
		m_array[indx] = m_array[indx-1];
	}

	m_array[0] = item;
	++m_count;

	return m_count;
}

template <class T>
AFX_INLINE int
CDynamicArray<T>::Append( T item )
{
	if ((m_count+1) >= m_size)
		Grow( m_delta );

	m_array[m_count] = item;
	++m_count;

	return m_count;
}

template <class T>
AFX_INLINE int
CDynamicArray<T>::ConditionalAppend( T item )
{
	int status = Find( item );

	if (status < 0)
		status = Append( item );

	return status;
}


template <class T>
int
CDynamicArray<T>::InsertBefore( int indx, T item )
{
	if ((m_count+1) >= m_size)
		Grow( m_delta );

	for (int jndx = m_count; jndx > indx; --jndx)
	{
		m_array[jndx] = m_array[jndx-1];
	}

	m_array[indx] = item;
	++m_count;

	return m_count;
}

template <class T>
int
CDynamicArray<T>::InsertAfter( int indx, T item )
{
	if (indx == m_count)
		Append( item );
	else
		InsertBefore( (indx+1), item );

	return m_count;
}

template <class T>
T
CDynamicArray<T>::Replace( int indx, T item )
{
	T prevItem = m_array[ indx ];
	m_array[indx] = item;

	return prevItem;
}

template <class T>
T
CDynamicArray<T>::Remove( int indx )
{
	T theData = m_array[ indx ];

	for (int jndx = indx; jndx < (m_count-1); ++jndx)
	{
		m_array[jndx] = m_array[jndx+1];
	}

	m_array[m_count-1] = NULL;
	--m_count;

	return theData;
}

// TODO: Test!!!!!
template <class T>
bool CDynamicArray<T>::Qsort( int (__cdecl *compare)(const void *elem1, const void *elem2 ) )
{
	bool okay = true;

	if (m_count > 1)
	{
		try
		{
			int indx;

			T* temp = new T[m_count];

			for (indx = 0; indx < m_count; ++indx)
			{
				temp[indx] = m_array[indx];
				m_array[indx] = NULL;
			}

			qsort( temp, m_count, sizeof(T), compare );

			for (indx = 0; indx < m_count; ++indx)
			{
				m_array[indx] = temp[indx];
				temp[indx] = NULL;
			}

			delete [] temp;
		}
		catch ( CException* e )
		{
			e->Delete();		//Must NEVER user standard delete on CExceptions

			CReturn status;
			status.Internal( IDS_INTERNAL_ERROR, "CDynamicArray<T>::Qsort()" );
			okay = false;
			delete e;
		}
	}

	return okay;
}

template <class T>
void
CDynamicArray<T>::BenignFlush()
{
	for (int indx = 0; indx < m_count; ++indx)
	{
		m_array[indx] = NULL;
	}
	m_count = 0;
}

template <class T>
void
CDynamicArray<T>::DestructiveFlush()
{
	for (int indx = 0; indx < m_count; ++indx)
	{
		delete m_array[ indx ];
		m_array[indx] = NULL;
	}
	m_count = 0;
}

template <class T>
bool
CDynamicArray<T>::BinarySearch(
							void*	myItem,
							int		(*func)( void* myItem, void* arrayItem ),
							int*	indx ) const
{
	int lo = 0;
	int hi = m_count - 1;

	while (1)
	{
		if (hi < lo)
			break;  // not found

		int mid = (lo + hi) / 2;

		T arrayItem = m_array[mid];

		int dir = SGN( (*func)( myItem, arrayItem ) );

		switch( dir )
		{
		case 0:
			(*indx) = mid;
			return TRUE;  // found
		case -1:
			lo = mid + 1;
			break;
		case 1:
			hi = mid - 1;
			break;
		}
	}

	(*indx) = lo;
	return FALSE;
}

template <class T>
void
CDynamicArray<T>::Grow( int delta )
{
	T* new_array = new T[ m_size+delta ];
	if (new_array != NULL)
	{
		m_size += delta;

		memset( new_array, 0, sizeof(T) );

		for (int indx = 0; indx < m_count; ++indx)
		{
			new_array[indx] = m_array[indx];
		}

		delete [] m_array;
		m_array = new_array;
	}
}

#endif