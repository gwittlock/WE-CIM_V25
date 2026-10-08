#if !defined(_GEOPOLY_H)
#define _GEOPOLY_H
#pragma once

// ==================================================================
//		GeoPoly
//
//	This is a geometric polygon -- a figure composed of a series
//	of points with implied line connections.
//
//	Poly's have a variety of powerful testing options, specifically,
//	true-shape inclusion and exclusion of each other.
//
//	The main purpose of this entity is to perform inclusion / exclusion
//	tests so all geometry is X,Y only.
//
//	ALSO includes boolean operations on polygons.
//
//	Code based closely on PolySolid2 code from Magic Software, Inc.
//		http://www.magic-software.com
//		http://www.magic-software.com/License/free.pdf
//
//	Algorithm is (not well) described in David Eberly's paper,
//	"Polysolids and Boolean Operations" at 
//	http://www.magic-software.com/License/free.pdf
//
//	But this is just a version of the algorithm for constructive
//	planar geometry described by Michael Leonov for his poly_Boolean
//	software (found at http://www.afti.nsu.ru/~leonov/clipdoc.html).
//
//	Of course, this is just an extension of the older work by Klamer
//	Schutte, as described in "An edge labeling approach to concave polygon
//	clipping" as found at http://www.ph.tn.tudelft.nl/~klamer/Klamer.html.
//
//	This, in turn, is founded on the decomposition and classification of
//	polygons using BSP trees... 
//
//	Our implementation, however, also includes native treatment for arcs.
// ==================================================================


#ifndef _DYNAMICARRAY_H
#include "DynamicArray.h"
#endif

#include "Common.h"
#include "3dCoord.h"
#include "GeoCurve.h"

#include "PolyEdge.h"

// ==================================================================

const double BLOAT_OFFSET = 0.01;

class dllExport CGeoPolyArray;

// ==================================================================
// TODO:  Note the overlaps and redundancies between GeoPoly, Profile, Shape, and ToolShape
//	It may be possible to make these more efficient, especially as I add tons
//	of new stuff to GeoPoly.  eww  18 Jun 2002
//
// 2008.01.29 (PE) -- CGeoPoly now isA CGeoElem. It *is not* a CGeoCurve because
// doing that (currently) requires too much effort (because CGeoCurve has numerous
// pure-virtual functions that must be satisfied).
//
class dllExport CGeoPoly : public CGeoElem
{
public:

	CGeoPoly();
	virtual ~CGeoPoly();

	CGeoPoly( const C2dBox& box );

	CGeoPoly& operator= (const CGeoPoly& poly);

	void Flush( void );
	void BenignFlush();

	// Creation
	CReturn CopyAppend( const CGeoCurve& curve );
	CReturn CopyAppend( const CGeoPoly& poly );

	void Append( CGeoCurve* curve );

	// Manipulation
	void Reverse( void );
	void Shift( int delta );  // really only makes sense for closed but ...
	void Reduce();
	void LinesReduce( bool wrap, double tol );
	void ArcsReduce( bool wrap, double tol );
	void TrimLoops( int max_loop, double tol);

	CGeoPoly* ExtractProfile( int dir, double gap_tol, bool creating_remnant );

	bool Split( int indx, const C2dCoord& pt );

	// Queries.
	virtual bool IsClosed( double tol ) const;
	bool IsConvex() const;
	bool IsCircle( double tol ) const;  // a convenience method

	// Special method introduced for remnant creation.
	void GapsClose();
	//
	// Relativity Operations
	//
	bool Encloses( CGeoPoly& poly );
	bool Overlaps( CGeoPoly& poly );
	bool Excludes( const CGeoPoly& poly );

	bool PtInPoly( const C2dCoord& pnt ) const;

#if (_CI || _NST)
	bool CI_Encloses( CGeoPoly& poly );
	bool CI_Overlaps( CGeoPoly& poly );
#endif

    // Boolean Operations
	CGeoPoly* AND(CGeoPoly& poly);
	CGeoPoly* OR(CGeoPoly& poly);
	CGeoPoly* MINUS(CGeoPoly& poly);

	void AttribsPropogate( bool propogate )  { propogate; /* compiler fodder, used to be m_propogate = propogate; */ }
	bool AttribsPropogate() const            { return m_propogate; }

	// Internal Access

	int	Count( void ) const		{ return m_curves.Count(); }

	const CGeoCurve& operator []( int idx ) const	{ return *(m_curves[idx]); }
	const CGeoCurve& GetAt( int idx ) const			{ return *(m_curves[idx]); }

	const CGeoCurveArray& Curves() const  { return m_curves; }

	const C2dBox& Extent( void ) const;	// Extent not Box, since not related GeoElem

	double Area( void ) const		{ return m_area; }
	int Winding( void ) const		{ return SGN(m_area); }

	const C2dCoord	Center( void ) const		{ return C2dCoord(m_extent.Xc(), m_extent.Yc()); }

	// Not to be confused with CGeoCurve::PointClosest().
	double PointClosest( const C3dCoord& pt, C3dCoord* closest ) const;

	//--- virtual ---

	virtual ElemType Type() const;

	virtual CGeoElem* Clone( bool attribs_copy ) const;

	virtual const C3dCoord& StartPt() const;

	virtual void StartPt( const C3dCoord& pt );
	virtual void StartPt( double xs, double ys, double zs );

	virtual const C3dCoord& EndPt() const;

	virtual void EndPt( const C3dCoord& pt );
	virtual void EndPt( double xe, double ye, double ze );

	virtual double Length2d() const;

	const C3dBox& Box() const;

	virtual void Xform( const C3x4Matrix& xform );
	virtual void Shift( const C3dVec& delta );


	void Tabulate(
		double			chordal_tol,
		const C3dVec&	shift,
		C3dCoordArray*	pts ) const;

	// For debugging.
	void Trace() const;
	void Dump() const;

public:

	static C2dBox Extent( CGeoPolyArray* array );
	static double Area( CGeoPolyArray* array );

	static CGeoPoly* ConvexHull( const C3dCoordArray& pts );

private:

	// Disabled.
	CGeoPoly( const CGeoPoly& );
	int operator == ( const CGeoPoly& ) const;
	int operator != ( const CGeoPoly& ) const;

private:

	void	offset( const C2dCoord& in_pt, C2dCoord* out_pt, double off );
	int		get_quadrant( const C2dCoord& ctr, const C2dCoord& pt );
	CReturn	do_append( const CGeoCurve& curve );

	void	replace_curves( CGeoCurveList* curves );

	double	sub_area( const CGeoCurve& curve ) const;

    void Segment( const CGeoPoly& poly, CPolyEdge segedge[4] );

	int rightmost_entity( const CGeoCurveArray& curvelist );

private:

	// CGeoCurveList*	m_curvelist;
	CGeoCurveArray	m_curves;

	C2dBox			m_extent;	// Extent and Area are maintained via do_append
	double			m_area;	

	bool			m_polyp_trim;
	bool			m_propogate;
};

class dllExport CGeoPolyArray : public CDynamicArray<CGeoPoly*>
{
public:

	CGeoPolyArray()  { }
	virtual ~CGeoPolyArray()  { }

	void Draw() const;
};

#endif

