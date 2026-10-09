#if !defined(_SEEDLIST_H)
#define _SEEDLIST_H
#pragma once

// ==================================================================
//		SeedList
//
// ==================================================================

#include "Common.h"
#include "Seed.h"

class CNestConfig;
class CSheet;

// ==================================================================

class dllExport CSeedList
{
public:

	CSeedList( int size );
	~CSeedList( void );

	// ---------------------

	int	Size( void ) const		{ return m_size; }
	int	Count( void ) const		{ return ((m_tail - m_head) + m_size) % m_size; }

	void Reset( void );

	// 2006.10.03 (PE)
	void ReduceToPriority( int priority_mask );

	const CSeed&	Head( void ) const		{ return m_list[m_head]; }
	const CSeed&	PopTail( void );

	const CSeed&	PeekTail( void ) const		{ return m_list[m_tail-1]; }

// If we re-instante PopHead, we MUST FIX Reduce() to work with wrap-arounds
//	const CSeed&	PopHead( void )			{ CSeed& seed = m_list[m_head];  m_head = (m_head+1)%m_size; return seed; }
#if REQUIRED
	bool	Prepend( const CSeed& seed );	// Copies contents
#endif
	bool	Append( const CSeed& seed );	// Copies contents

	bool Qsort(
		eNestProgression	progression,
		const CSheet&		sheet );

	void	Reduce( double range );

	void	Shift( C2dVec delta );

	// Debugging interface
	int debug_head() const { return m_head; }
	int debug_tail() const { return m_tail; }
	int debug_size() const { return m_size; }
	CSeed& debug_seed(int idx) { return m_list[idx]; }

	void Dump();

	//=-=-=-=-=-

	void MyReset();
	bool AtEnd() const;

	// DO NOT CALL unless AtEnd() returns false!!!!
	const CSeed& Next();

private:

	void GridScore(
			eNestProgression	progression,
			const CSheet&		sheet,
			CSeed*				seed );

private:
	CSeed*	m_list;
	int		m_size;

	int		m_head;
	int		m_tail;

	int		m_curr;
};


#endif
