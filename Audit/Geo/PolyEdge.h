#if !defined(_POLYEDGE_H)
#define _POLYEDGE_H
#pragma once

// ==================================================================
//	PolyEdge
//
//	Slave utility class of CGeoPoly and CPolySegment.  A list manager
//	to keep track of various curves; includes a variety of ways of
//	adding a new "edges" to the list.
//
//	Maintains a list of curves (er, edges) in an un-ordered list
//
// ==================================================================

#include "GeoLine.h"
#include "GeoArc.h"
#include "GeoCurve.h"

// ==================================================================

class CGeoPoly;	// Can't include; causes a loop

class CPolyEdge
{
public:

    CPolyEdge();
    virtual ~CPolyEdge();

	int Count( void ) const					{ return m_edge_list.Count(); }
	CGeoCurve* operator[](int idx)				{ return m_edge_list[idx]; }

	void AddEdge( const CGeoCurve& curve );

	void Reset( void )						{ m_edge_list.DestructiveFlush(); }

	// Various ways of adding edges...
    void MergeAppend( CPolyEdge& edge, bool reverse );
    void MergeUnique( CPolyEdge& edge, bool reverse );
    void MergeEqual( CPolyEdge& edge );

    CGeoCurveList*	Curves()					{ return &m_edge_list; }

	bool HasAttrib() const			{ return (m_attribs != NULL); }

	// Low-level methods (caution).
	void Attribs( const CVarList* attribs )  { m_attribs = attribs; }

	void Draw() const  { m_edge_list.Draw(); }

private:

	bool within_tol( const CGeoCurve& c1, const CGeoCurve& c2, double tol );
	
private:

	CGeoCurveList m_edge_list;

	const CVarList* m_attribs;
};

#endif
