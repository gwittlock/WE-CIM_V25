#if !defined(_DEX_H)
#define _DEX_H
#pragma once

// ==================================================================
//		DexGrid
//
// ==================================================================

#include "Common.h"
#include "IndxList.h"


// ==================================================================

#pragma pack( push, 1 )
class dllExport CDex
{
public:
	CDex( void );
	CDex( int type, WORD first, WORD last );
	~CDex( void );

	int	Type( void ) const		{ return m_type; }
	void	Type( int val )		{ m_type = val; }

	WORD	First( void ) const		{ return m_first; }
	void	First( WORD val )		{ m_first = val; }

	WORD	Last( void ) const		{ return m_last; }
	void	Last( WORD val )		{ m_last = val; }

	WORD	Delta() const			{ return (m_last - m_first + 1); }

	CDex*	Next( void ) const		{ return m_next; }
	void	Prepend( CDex* prev )	{ prev->m_next = this; }	// ONLY USE TO REPLACE HEAD
	void	Append( CDex* next )	{ next->m_next = m_next; m_next = next; }

	void	MergeNext( void );
	void	Insert( int type, int origin );

	void	Debug( HANDLE in_file, bool links=FALSE ) const;

private:
	int	m_type;		// 1
	WORD	m_first;	// 2 Index of first "grid" point in span
	WORD	m_last;		// 2 Index of last "grid" point in span (may equal start)
	CDex*	m_next;		// 4 TODO:  Turn into WORD index into a CDexPool manager
};

#pragma pack( pop )

typedef CIndxList<CDex*> CDexList;
typedef CDynamicArray<CDex*> TDexArray;

#endif
