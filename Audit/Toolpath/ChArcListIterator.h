
#ifndef _CHARCLISTITERATOR_H
#define _CHARCLISTITERATOR_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef _CHARC_H
#include "ChArc.h"
#endif


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class CChArcListIterator
{
public:

	CChArcListIterator();

	CChArcListIterator( const CChArcList& arcList );

	void Init( const CChArcList& arcList );

	// Access operator.
	const CChArc* operator()() const;

	// Returns true when the iterator is positioned
	// at the last arc in the arc list.
	bool AtEnd() const;

	// Simply retreat to the previous arc.
	const CChArc* Prev();

	// Simply advance to the next arc.
	const CChArc* Next();

	// Advance the iterator to the next arc whose
	// convolution of overlap agrees with the given arc.
	int Advance( const CChArc* refArc, int offsetDir );

	// Advance the iterator to 'forward most' arc whose
	// convolution of overlap agrees with the given arc.
	int StartPosition( const CChArcListIterator& partIter, int offsetDir );

	const CChArcList& List() const  { return (*m_list); }

	virtual ~CChArcListIterator();

protected:

private:  // Methods

	int PrevIndex( int indx );
	int NextIndex( int indx );

private:  // Disabled

	CChArcListIterator( const CChArcListIterator& );
	const CChArcListIterator& operator = ( const CChArcListIterator& );
	int operator == ( const CChArcListIterator& ) const;
	int operator != ( const CChArcListIterator& ) const;

private:  // Data

	const CChArcList* m_list;
	int m_indx;
};

#endif

