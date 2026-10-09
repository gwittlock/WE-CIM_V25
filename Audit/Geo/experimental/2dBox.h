
#ifndef _2DBOX_H
#define _2DBOX_H

#ifndef _2DCOORD_H
#include "2dCoord.h"
#endif

class CGeoLine;

class C3dBox;

class dllExport C2dBox
{
public:

	C2dBox();

	C2dBox( double xmin, double ymin, double xmax, double ymax );

	C2dBox( const C2dCoord& ps, const C2dCoord& pe );

	C2dBox( const C2dBox& box );
	C2dBox( const C3dBox& box );

	const C2dBox& operator = ( const C2dBox& box );
	const C2dBox& operator = ( const C3dBox& box );

	void OrdinatesSet( double xa, double ya, double xb, double yb );

	// Returns TRUE if the box is uninitialized.
	bool IsDefined() const;

	// Obtain an ordinate of the bounding box.
	double Xmin() const		{ return m_min.X(); }
	double Ymin() const		{ return m_min.Y(); }

	double Xmax() const		{ return m_max.X(); }
	double Ymax() const		{ return m_max.Y(); }

	C2dCoord TL() const		{ return C2dCoord( m_min.X(), m_max.Y() ); }
	C2dCoord BL() const		{ return C2dCoord( m_min.X(), m_min.Y() ); }

	C2dCoord TR() const		{ return C2dCoord( m_max.X(), m_max.Y() ); }
	C2dCoord BR() const		{ return C2dCoord( m_max.X(), m_min.Y() ); }

	// Update an ordinate of the bounding box.
	// Asserts if relationship between min & max values is violated.
	void Update( double xmin, double ymin, double xmax, double ymax );

	void Xmin( double x );
	void Ymin( double y );

	void Xmax( double x );
	void Ymax( double y );

	void X( double x );
	void Y( double y );

	// Obtain the center location of the bounding box.
	double Xc() const;
	double Yc() const;

	double Dx() const;
	double Dy() const;

	double Area() const;

	// Expand the bounding box to include the given point.
	const C2dBox& operator += ( const C2dCoord& pt );
	const C2dBox& operator += ( const C3dCoord& pt );

	// Expand the bounding box to include the given bounding box.
	const C2dBox& operator += ( const C2dBox& box );

	// Uniformly inflate/deflate the bounding box.
	const C2dBox& operator += ( double delta );
	C2dBox operator + ( double delta ) const;

	bool Contains( const C2dCoord& pt, double tol ) const;
	bool Contains( double x, double y, double tol ) const;

	bool Contains( const C2dBox& box, double tol ) const;
	bool ContainsXY( const C3dBox& box, double tol ) const;

	bool Intersects( const C2dBox& box, double tol ) const;	// Change to be consistent with 3d box

	bool Intersects( const CGeoLine& line, double tol ) const;

	void Shift(double dx, double dy);

	// Causes IsUndefined() to return TRUE.
	void Invalidate();

	virtual ~C2dBox();

private:
	bool do_intersect(double x0, double y0, double x1, double y1, double tol) const;
	int outcode(double x, double y, double tol) const;

private:
	// Store as Coord for better ability to index ordinates. eww.
	C2dCoord	m_min;
	C2dCoord	m_max;

};

typedef CDynamicArray<C2dBox*> C2dBoxArray;

#endif
