
#ifndef _3DBOX_H
#define _3DBOX_H

#ifndef _3DCOORD_H
#include "3dCoord.h"
#endif

#ifndef _2DBOX_H
#include "2dBox.h"
#endif


class dllExport C3dBox
{
public:

	C3dBox();

	C3dBox( double xmin, double ymin, double zmin, double xmax, double ymax, double zmax );

	C3dBox( const C3dCoord& ps, const C3dCoord& pe );

	C3dBox( const C3dBox& box );

	const C3dBox& operator = ( const C3dBox& box );

	const C3dBox& operator = ( const C2dBox& box );

	// Returns FALSE if the box is uninitialized.
	bool IsDefined() const;
	bool IsDefinedXY() const;

	// Obtain an ordinate of the bounding box.
	double Xmin() const		{ return m_min.X(); }
	double Ymin() const		{ return m_min.Y(); }
	double Zmin() const		{ return m_min.Z(); }

	double Xmax() const		{ return m_max.X(); }
	double Ymax() const		{ return m_max.Y(); }
	double Zmax() const		{ return m_max.Z(); }

	const C3dCoord& Min() const		{ return m_min; }
	const C3dCoord& Max() const		{ return m_max; }

	// Obtain a primary 'view' of the bounding box.
	C2dBox XY() const;
	C2dBox YZ() const;
	C2dBox ZX() const;

	// Update an ordinate of the bounding box.
	// Asserts if relationship between min & max values is violated.
	void Update( double xmin, double ymin, double zmin, double xmax, double ymax, double zmax );

	void Xmin( double x );
	void Ymin( double y );
	void Zmin( double z );

	void Xmax( double x );
	void Ymax( double y );
	void Zmax( double z );

	void X( double x );
	void Y( double y );
	void Z( double z );

	// Obtain the center location of the bounding box.
	double Xc() const;
	double Yc() const;
	double Zc() const;

	double Dx() const;
	double Dy() const;
	double Dz() const;

	// Expand the bounding box.
	const C3dBox& operator += ( const C3dCoord& pt );
	const C3dBox& operator += ( const C3dBox& box );

	void Shift( const C3dVec& delta );

	bool Contains( const C3dCoord& pt, double tol ) const;
	bool Contains( double x, double y, double z, double tol ) const;

	bool Contains( const C3dBox& box, double tol ) const;
	bool ContainsXY( const C3dBox& box, double tol ) const;
	bool Intersects( const C3dBox& box, double tol ) const;
	bool Intersects( const C2dBox& box, double tol ) const;

	// Causes IsUndefined() to return TRUE.
	void Invalidate();

	virtual ~C3dBox();

private:
	// Store as Coord for better ability to index ordinates. eww.
	C3dCoord	m_min;
	C3dCoord	m_max;
};


#endif
