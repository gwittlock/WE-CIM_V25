// ==================================================================
//		ViewStack
//
//	The ViewStack doesn't store views, but view transforms.
//	It allows the user to move back and forth among the most
// recently used transforms.
//
// ==================================================================

#include "stdafx.h"

#include "ViewStack.h"

// ==================================================================

CViewStack::CViewStack()
{
	m_indx = -1;
}

CViewStack::~CViewStack()
{
	Purge();
}

CViewXform*
CViewStack::newView( const C2dBox& extent, double scale )
{
	CViewXform* view_xform = NULL;

	if ( IsDifferentView( extent ) )
	{
		int count = m_view_stack.Count();
		if (count < (MAX_VIEWS - 1))
		{
			view_xform = new CViewXform();
		}
		else
		{
			// Lets do a little recycling.
			view_xform = m_view_stack.Remove(0);
			--m_indx;
		}
	}

	if (view_xform != NULL)
	{
		view_xform->ViewExtent( extent );
		view_xform->ViewScale( scale );
		m_view_stack.InsertAfter( m_indx, view_xform );
		++m_indx;
	}

	return view_xform;
}

void
CViewStack::Purge()
{
	m_view_stack.DestructiveFlush();
	m_indx = -1;
}

// ==================================================================
//		isNext
//
//	Returns TRUE if there is a "next" entry... FALSE if we are at the end
//
bool	
CViewStack::isNext()
{
	int count = m_view_stack.Count();
	return ((count > 0) && (m_indx < (count-1)));
}

// ==================================================================
//		isPrev
//
//	Returns TRUE if there is a "previous" entry... FALSE if we are at the beginning
//
bool
CViewStack::isPrev()
{
	int count = m_view_stack.Count();
	return ((count > 0) && (m_indx > 0));
}

// ==================================================================
CViewXform*
CViewStack::getNext()
{
	if ( isNext() )
		++m_indx;

	return ( m_view_stack.GetAt( m_indx ) );
}

// ==================================================================
CViewXform*
CViewStack::getPrev()
{
	if ( isPrev() )
		--m_indx;

	return ( m_view_stack.GetAt( m_indx ) );
}

bool
CViewStack::IsDifferentView( const C2dBox& extent )
{
	bool is_different = true;  // In case count is zero.

	int count = m_view_stack.Count();
	if (count > 0)
	{
		CViewXform* view_xform = m_view_stack.GetAt( m_indx );

		const C2dBox& curr = view_xform->ViewExtent();

		C2dCoord bl = curr.BL();
		C2dCoord tr = curr.TR();

		is_different = ( !bl.WithinTol( extent.BL(), 1.e-3 ) ||
			 !tr.WithinTol( extent.TR(), 1.e-3 ) );
	}

	return is_different;
}