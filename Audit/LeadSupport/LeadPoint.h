#if !defined(_LEADPOINT_H)
#define _LEADPOINT_H

#include "3dCoord.h"

// ==================================================================
//	LeadPoints
//
//	Attributed points used by the lead system while it's thinking
// ==================================================================

class dllExport CLeadPoint : public C3dCoord
{
public:
	CLeadPoint() : C3dCoord()		{ m_at_corner = false; }
	virtual ~CLeadPoint()			{}

	int		Index( void ) const		{ return m_profidx; }
	void	Index( int idx )		{ m_profidx = idx; }

	double	Score( void ) const		{ return m_score; }
	void	Score( double val )		{ m_score = val; }

	bool	doSplit( void ) const	{ return m_split; }
	void	doSplit( bool split )	{ m_split = split; }

	bool	AtCorner() const		{ return m_at_corner; }
	void	AtCorner( bool at_corner )		{ m_at_corner = at_corner; }

private:

	int		m_profidx;
	double	m_score;
	bool	m_split;
	bool	m_at_corner;
};

typedef CDynamicArray<CLeadPoint*>	CLeadPointArray;

#endif