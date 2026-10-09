#if !defined(_LINK_H)
#define _LINK_H
#pragma once

#include "Common.h"
#include "2dCoord.h"
#include "2dbox.h"

// ==================================================================
//
// Bitfields for the link flags
//
const DWORD CONFIG_PRELINK		= 0x00000001;

// ==================================================================

class dllExport CLink
{
public:

	CLink();
	CLink( const CLink& rhs );
	CLink(int hit, double priority, C2dVec& delta);
	~CLink();

	CLink& operator = ( const CLink& rhs );

	bool IsEqual( const CLink& rhs, double tol );

	int Hit() const					{ return m_hit; }
	void Hit(int hit)				{ m_hit = hit; }

	double Priority() const			{ return m_priority; }
	void Priority(double val)		{ m_priority = val; }

	const C2dVec& Delta() const		{ return m_delta; }
	void Delta(const C2dVec& vec)	{ m_delta = vec; }
	void Delta(double x, double y)	{ m_delta.Init( x, y ); }

	double dX() const				{ return m_delta.X(); }
	double dY() const				{ return m_delta.Y(); }

	// Flags

	bool PreLink() const			{ return m_prelink; }
	void PreLink( bool set )		{ m_prelink = set; }

	void BumpScore( double score )	{ m_bump_score = score; }
	double BumpScore() const		{ return m_bump_score; }

	void OverlapScore( double score )	{ m_overlap_score = score; }
	double OverlapScore() const			{ return m_overlap_score; }

private:  // disabled

	int operator == ( const CLink& ) const;
	int operator != ( const CLink& ) const;

private:

	int			m_hit;			// Link TO member of same family
	C2dVec		m_delta;		// X, Y offset of the link
	double		m_priority;		// Priority scoring factor

	bool		m_prelink;
	double		m_bump_score;
	double		m_overlap_score;
};

class dllExport CLinkArray :public CDynamicArray<CLink*>
{
public:

	CLinkArray()   { }
	~CLinkArray()  { }

	void Sort();

private:

	static int DecreasingScore( const void* ptrA, const void* ptrB );
};

#endif
