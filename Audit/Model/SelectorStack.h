
#ifndef _SELECTORSTACK_H
#define _SELECTORSTACK_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef _INDXLIST_H
#include "IndxList.h"
#endif

#ifndef _SELECTOR_H
#include "Selector.h"
#endif

class CModel;


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class dllExport CSelectorStack
{
public:

	CSelectorStack();

	// Sets the stack's reference to a model and
	// pushes a reserved selector onto the stack.
	void Init( CModel& model );

	// Flushes the stack and clears and clears
	// the 'selected' flag of the entities.
	void Flush();

	// Access operator to the selector on the top of the stack.
	CSelector& operator()();

	// Push a new selector onto the stack.
	// Clears the 'selected' flag of any
	// entities in the previous stack frame.
	void Push();

	// Pop the topmost selector off of the stack.
	// Restores the 'selected' flag of any
	// entities in the new topmost stack frame.
	void Pop();

	virtual ~CSelectorStack();

protected:

private:  // Methods

	// Set the selection state of the entities
	// in the topmost stack frame.
	void TopmostState( bool select );

private:  // Disabled

	CSelectorStack( const CSelectorStack& );
	const CSelectorStack& operator = ( const CSelectorStack& );
	int operator == ( const CSelectorStack& ) const;
	int operator != ( const CSelectorStack& ) const;

private:  // Data

	CIndxList<CSelector*> m_list;

	CModel* m_model;
};

#endif

