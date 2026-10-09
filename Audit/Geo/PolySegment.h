#if !defined(_POLYSEGMENT_H)
#define _POLYSEGMENT_H
#pragma once

// ==================================================================
//		CPolySegment
//
//	Slave utility class of CPolyBool.  Used during segmentation of
//	a line with respect to a poly.
//
//	Preserves a list of "hit" points in order along the line.
//
// ==================================================================

#include "GeoCurve.h"
#include "PolyEdge.h"
#include "GeoPoly.h"

// ==================================================================

enum eSegTag
{ 
	OTAG = 0, 
	ITAG = 1, 
	MTAG = 2, 
	PTAG = 3
};

struct sTaggedPoint
{
	double		m_uparam;
	C2dCoord	m_pnt;
	eSegTag		m_tag;
};

const double SEGMENT_TOL = 5e-5;

// ==================================================================

class CPolySegment
{
public:
    CPolySegment( const CGeoCurve& curve );
    virtual ~CPolySegment();

    int Count(void) const		{ return m_tpnt_list.Count(); }

    void SegmentBy( const CGeoPoly& poly );
    void Reduce( const CGeoCurve& curve );

    void ConvertToEdges ( CPolyEdge segedge[4] );

	bool HasAttrib() const			{ return (m_attribs != NULL); }

	// Low-level methods (caution).
	void Attribs( const CVarList* attribs )  { m_attribs = attribs; }

private:

    void insert_pnt( const C2dCoord& pnt, eSegTag tag );
	void convert_to_edges( const CGeoArc& arc, CPolyEdge segedge[4] );
	void convert_to_edges( const CGeoLine& line, CPolyEdge segedge[4] );
	int do_intersect( const CGeoCurve& edge, C2dCoord hit[], bool extended );

private:  // disabled

	CPolySegment();
	CPolySegment( const CPolySegment& rhs );

private:

    static eSegTag	m_klein4[4][4];

private:

	CIndxList <sTaggedPoint*> m_tpnt_list;

    const CGeoCurve& m_curve;
	CGeoLine m_line;

	const CVarList* m_attribs;
};

#endif
