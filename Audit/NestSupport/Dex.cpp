// ==================================================================
//		DexGrid
//
// ==================================================================

#include <stdafx.h>

#include "Grid.h"
#include "Dex.h"

// ==================================================================

CDex::CDex( void )
	: m_type(0),
	  m_first(0),
	  m_last(0),
	  m_next(NULL)
{

}

CDex::CDex( int type, WORD first, WORD last )
	: m_type(type),
	  m_first(first),
	  m_last(last),
	  m_next(NULL)
{
	if (first > last)
	{
		bool bomb = TRUE;
	}

}


CDex::~CDex( void )
{             
	// Can't delete the thread of dex here, or it eats
	// stuff in use!
	m_next = NULL;
}

void CDex::MergeNext( void )
{
	CDex* next = m_next;

	if (!next)
	{ return; }

	Last( next->Last() );

	m_next = next->Next();
	delete next;
}


// ==================================================================
//		Insert
//
//	ASSERT( origin >= m_first );
//	ASSERT( origin <= (m_last+1) );
//
void CDex::Insert( int type, int origin )
{
	// The new type may be the same or different from our current type.
	// If the type is different, we *may* need to stretch out dexel a bit.
	//
	CDex* new_dex = NULL;
	if (type == m_type)
	{
		if (origin == (m_last + 1))
		{
			// Grow by one...
			m_last++;
			if (m_next)
			{
				m_next->First( origin+1 );

				// Did we just kill next?
				if (m_next->First() > m_next->Last())
				{
					CDex* kill = m_next;
					m_next = kill->Next();
					delete kill;
				}
			}
		}
	}
	else
	{
		// A different type... there are four options:
		//	1. replace this dex (if it's length is one)
		//	2. put new dex at head of this dex
		//	3. put new dex inside this dex (splitting it)
		//	4. put new dex at tail of this dex (possibly adjusting next dex, too)
		//
		// This code may be a bit redundant, but I prefer it to be easy to read than
		// perfectly factored.
		//
		if (origin == m_first)
		{
			if (origin == m_last)
			{
				// REPLACE
				m_type = type;
				if ( m_next
					&& (type == m_next->Type()) )
				{ MergeNext(); }
			}
			else
			{
				// HEAD
				new_dex = new CDex( m_type, m_first+1, m_last );

				Type( type );
				Last( origin );

				Append( new_dex );
			}
		}
		else
		if (origin >= m_last)
		{
			// TAIL
			m_last = origin-1;
			if ( m_next
				&& (type == m_next->Type()) )
				m_next->First( origin );
			else
			{
				if ( m_next
					&& (origin == m_next->First())
					&& (origin == m_next->Last()) )
				{
					m_next->Type( type );
				}
				else
				{
					new_dex = new CDex( type, origin, origin );
					if (m_next)
						m_next->First( origin + 1 );

					new_dex->m_next = m_next;
					m_next = new_dex;
				}
			}
		}
		else
		{
			// SPLIT
			WORD last = m_last;
			CDex* next = m_next;
			m_last = origin-1;

			new_dex = new CDex( type, origin, origin );
			Append( new_dex );

			CDex* split_dex = new CDex( m_type, origin+1, last );
			new_dex->Append( split_dex );
		}
	}
}

void CDex::Debug( HANDLE file, bool links ) const
{
	const CDex* dex = this;
	const CDex* prev = this;

	CString buf;
	DWORD did_write;
	buf.Format( "%04d ", dex->m_first );
	WriteFile( file, (LPCVOID)buf, buf.GetLength(), &did_write, NULL );

	buf = "";
	while (dex)
	{
		int type = dex->Type();
		char mark = GridSymbolGet( type );

		for (int at_x=dex->First(); at_x<=dex->Last(); at_x++)
			buf += mark;

		// Used for some Debug*() to mark spaces between dexels
		if (links)
			buf += "_";

		prev = dex;
		dex = dex->Next();
	}

	CString tail;
	tail.Format( " %04d", prev->m_last );

	buf += tail;
	buf += "\n";
	WriteFile( file, (LPCVOID)buf, buf.GetLength(), &did_write, NULL );
}
