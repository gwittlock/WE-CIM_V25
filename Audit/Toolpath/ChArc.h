
#ifndef _CHARC_H
#define _CHARC_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef _INDXLIST_H
#include "IndxList.h"
#endif

#ifndef _2DCOORD_H
#include "2dCoord.h"
#endif

#ifndef _2DUNITVEC_H
#include "2dUnitVec.h"
#endif

class CChArc;
typedef CIndxList<CChArc*> CChArcList;

const int CHARC_TURN = 1;
const int CHARC_MOVE = 2;

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// See also Docs/Minkowski.htm
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Convex-Hull Arc
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Based on the paper 'An Alogrithm to Compute the Minkowski Sum Outer-face
// of Two Simple Polygons' by G.D.Ramkumar, Dept. of CS, Stanford.
//
// A 'kinetic framework' for this computation is described by:
//
//    'A state is a pair consisting of a position in the plane and a
//     direction.  A move is a set of states with constant direction
//     and position varying along a line segment parallel to the
//     direction.  A turn is a set of states with constant position
//     and direction varying along an arc of the circle of directions.'
//
// A state pair is modelled using a CChArc object.  In the spirit of the
// preceeding description, a move is modelled as an arc that has no change
// in the starting and ending direction vectors.  Likewise, a turn is
// modelled as an arc that has some change between its starting and ending
// direction vectors.
//
// It should be noted that degenerate arcs can appear in the convolution.
// Known degeneracies are:
//
// 1) An arc of zero radius and a change in direction vectors
//    (indicative of a turn around a sharp corner).
//
// 2) An arc of zero or greater radius and no change in direction vectors
//    (indicative of a line end point).
//
// 3) An arc of negative radius
//    (indicative of a local loop).
//
class dllExport CChArc
{
public:

	CChArc(
			const C2dCoord& center,
			const C2dUnitVec& startTan,
			const C2dUnitVec& endTan,
			double radius,
			int dir );

	bool IsTurn() const;

	bool IsMove() const;

	double Radius() const;

	int Dir() const;

	void Dir( int dir );

	C2dCoord CenterPt() const;

	C2dCoord StartPt() const;

	C2dCoord EndPt() const;

	C2dUnitVec StartTan() const;

	C2dUnitVec EndTan() const;

	// Obtain the representation of this arc into its constituent
	// 'single quadrant' arcs.  NOTE: The client is responsible
	// for destroying the contents of chArcList
	void QuadrantArcs( CChArcList* chArcList ) const;

	virtual ~CChArc();

public:

	static bool SameDirection( const CChArc* partArc, const CChArc* toolArc );

	static int OverlapClassify( const CChArc* partArc, const CChArc* toolArc, int offsetDir );

protected:

private:  // Methods

	bool QuadIterDone( int dir, double ai, double ae ) const;

	void Angles( double* startAngle, double* endAngle ) const;

private:  // Disabled

	CChArc();
	CChArc( const CChArc& );
	const CChArc& operator = ( const CChArc& );
	int operator == ( const CChArc& ) const;
	int operator != ( const CChArc& ) const;

private:  // Data

	C2dCoord	m_pc;  // arc center point
	C2dUnitVec	m_ts;  // tangent at start point in arc direction
	C2dUnitVec	m_te;  // tangent at end point in arc direction
	double		m_rad;
	int			m_dir;
};


#endif

