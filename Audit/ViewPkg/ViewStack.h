#if !defined(_VIEWSTACK_H)
#define _VIEWSTACK_H

// ==================================================================
//		ViewStack
//
//	The ViewStack doesn't store views, but view transforms.
//	It allows the user to move back and forth among the most
// recently used transforms.
//
// ==================================================================

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "DynamicArray.h"
#include "ViewXform.h"

// ==================================================================

const int MAX_VIEWS = 20;

// ==================================================================

class dllExport CViewStack
{
public:

	CViewStack();
	virtual ~CViewStack();

	void Purge();

	CViewXform*	newView( const C2dBox& extent, double scale );

	// -------------------------------------------
	// List access
	//
	bool isNext();
	bool isPrev();

	CViewXform*	getNext();
	CViewXform* getPrev();
	
private:

	bool IsDifferentView( const C2dBox& extent );
	
private:

	// Disabled.
	CViewStack( const CViewStack& );
	const CViewStack& operator = ( const CViewStack& );
	int operator == ( const CViewStack& ) const;
	int operator != ( const CViewStack& ) const;
	
private:

	CDynamicArray <CViewXform*>	m_view_stack;
	int							m_indx;
};

#endif

