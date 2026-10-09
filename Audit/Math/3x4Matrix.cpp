// ==================================================================
//		3x4 Matrix
//
//	Used for transformations
//
// ==================================================================

#include "stdafx.h"
#include <math.h>
#include "MathConst.h"

#include "3x4Matrix.h"



static C3x4Matrix global_xform( 1., 0., 0.,
							    0., 1., 0.,
								0., 0., 1.,
								0., 0., 0. );


// ==================================================================

C3x4Matrix::C3x4Matrix()
	: m_i( 1.0, 0.0, 0.0 ),
	  m_j( 0.0, 1.0, 0.0 ),
	  m_k( 0.0, 0.0, 1.0 ),
	  m_t( 0.0, 0.0, 0.0 )
{
}
	
C3x4Matrix::C3x4Matrix( const C3x4Matrix& matrix )
	: m_i( matrix.m_i ),
	  m_j( matrix.m_j ),
	  m_k( matrix.m_k ),
	  m_t( matrix.m_t )
{
}

C3x4Matrix::~C3x4Matrix()
{
}

C3x4Matrix::C3x4Matrix( 
	const C3dVec& in_i,
	const C3dVec& in_j,
	const C3dVec& in_k,
	const C3dCoord& in_t )
	:	m_i( in_i ),
		m_j( in_j ),
		m_k( in_k ),
		m_t( in_t )
{
}

C3x4Matrix::C3x4Matrix( double ix, double iy, double iz,
						double jx, double jy, double jz,
						double kx, double ky, double kz,
						double tx, double ty, double tz )
	: m_i( ix, iy, iz ),
	  m_j( jx, jy, jz ),
	  m_k( kx, ky, kz ),
	  m_t( tx, ty, tz )

{
}


// =======================================================================
//		Assignment operator
//

const C3x4Matrix& 
C3x4Matrix::operator=( 
	const C3x4Matrix& in_mat )
{
	m_i = in_mat.m_i;
	m_j = in_mat.m_j;
	m_k = in_mat.m_k;
	m_t = in_mat.m_t;

	return *this;
}


// =======================================================================
//		setUnit
//
//		Reset the matrix to be a unit matrix -- no transform
//
void
C3x4Matrix::setUnit( void )
{
	m_i = C3dVec( 1, 0, 0 );
	m_j = C3dVec( 0, 1, 0 );
	m_k = C3dVec( 0, 0, 1 );

	m_t = C3dCoord( 0, 0, 0 );
}

// =======================================================================
//		setXYAngle
//
//		Reset the matrix, rotated around the Z axis (on the XY plane)
//
void	
C3x4Matrix::setXYAngle( 
	double in_ang )
{
	double	ca;
	double	sa;

	ca = cos(in_ang);
	sa = sin(in_ang);

	m_i = C3dVec(  ca, sa, 0 );
	m_j = C3dVec( -sa, ca, 0 );
	m_k = C3dVec(  0,  0,  1 );

	m_t = C3dCoord(  0,  0,  0 );
}

// =======================================================================
//		setXZAngle
//
//		Reset the matrix, rotated around the Y axis (on the XZ plane)
//
void	
C3x4Matrix::setXZAngle( 
	double in_ang )
{
	double	ca;
	double	sa;

	ca = cos(in_ang);
	sa = sin(in_ang);

	m_i = C3dVec( ca, 0, -sa );
	m_j = C3dVec( 0,  1, 0 );
	m_k = C3dVec( sa, 0, ca );

	m_t = C3dCoord( 0,  0, 0 );
}

// =======================================================================
//		setYZAngle
//
//		Reset the matrix, rotated around the X axis (on the YZ plane)
//
void	
C3x4Matrix::setYZAngle( 
	double in_ang )
{
	double	ca;
	double	sa;

	ca = cos(in_ang);
	sa = sin(in_ang);

	m_i = C3dVec( 1,  0,  0 );
	m_j = C3dVec( 0,  ca, sa );
	m_k = C3dVec( 0, -sa, ca );

	m_t = C3dCoord( 0,  0,  0 );
}

// =======================================================================
//		Scale
//
//		Scale the transform matrix, that is, cause it to scale
//
void	
C3x4Matrix::Scale( 
	double in_scale )
{
	m_i = m_i * in_scale;
	m_j = m_j * in_scale;
	m_k = m_k * in_scale;
}


void	
C3x4Matrix::Scale( 
	double in_scale_x,
	double in_scale_y,
	double in_scale_z )
{
	m_i = m_i * in_scale_x;
	m_j = m_j * in_scale_y;
	m_k = m_k * in_scale_z;
}

// =======================================================================
//		Shift
//
//		Change the translation component.. relative
//
void
C3x4Matrix::Shift( 
	const C3dVec&	in_shift )
{
	m_t += in_shift;
}

// =======================================================================
//		InvertTo
//
//		Inverting the matrix causes the transform to work in reverse...
//		at least for well-formed matrices.
//
//		Inverts to the specified matrix.
//
void
C3x4Matrix::InvertTo( C3x4Matrix* io_mat ) const
{
	C3dCoord	trans;

	// Invert rotational component
	io_mat->m_i.X( m_i.X() );
	io_mat->m_j.Y( m_j.Y() );
	io_mat->m_k.Z( m_k.Z() );

	io_mat->m_i.Y( m_j.X() );
	io_mat->m_j.X( m_i.Y() );
	io_mat->m_i.Z( m_k.X() );

	io_mat->m_k.X( m_i.Z() );
	io_mat->m_j.Z( m_k.Y() );
	io_mat->m_k.Y( m_j.Z() );

	// Mangle translational component
	trans.X( -m_t.X() );
	trans.Y( -m_t.Y() );
	trans.Z( -m_t.Z() );

	io_mat->m_t.X( (io_mat->m_i.X() * trans.X())
						 + (io_mat->m_j.X() * trans.Y())
						 + (io_mat->m_k.X() * trans.Z()) );

	io_mat->m_t.Y( (io_mat->m_i.Y() * trans.X())
						 + (io_mat->m_j.Y() * trans.Y())
						 + (io_mat->m_k.Y() * trans.Z()) );

	io_mat->m_t.Z( (io_mat->m_i.Z() * trans.X())
						 + (io_mat->m_j.Z() * trans.Y())
						 + (io_mat->m_k.Z() * trans.Z()) );
}


// =======================================================================
//		Transform
//
//		Transform a point by the matrix, in place
//
void	
C3x4Matrix::Transform( C3dCoord* io_pnt ) const
{
	C3dCoord	tmp;

	TransformTo( *io_pnt, &tmp );
	*io_pnt = tmp;
}

void	
C3x4Matrix::Transform( C2dCoord* io_pnt ) const
{
	C2dCoord	tmp;

	TransformTo( *io_pnt, &tmp );
	*io_pnt = tmp;
}

void	
C3x4Matrix::Transform( C3dVec* io_vec ) const
{
	C3dVec tmp;

	TransformTo( *io_vec, &tmp );
	*io_vec = tmp;
}

void	
C3x4Matrix::Transform( C2dVec* io_vec ) const
{
	C3dVec tmpA = *io_vec;
	C3dVec tmpB = *io_vec;

	TransformTo( tmpA, &tmpB );
	*io_vec = tmpB;
}


// =======================================================================
//		Transform
//
//		Transform the given matrix by this one... multiply the matrices
//		Occurs in place (this matrix is modified)
//
//		The transformations of the two matrices will occur in a distinct
//		order:
//
//		The current matrix will occur first
//		The passed-in matrix will occur second
//
void
C3x4Matrix::Transform( C3x4Matrix* io_mat ) const
{
	C3x4Matrix	tmp;

	TransformTo( *io_mat, &tmp );
	*io_mat = tmp;
}


// =======================================================================
//		TransformTo
//
//		Transform a point by the matrix, into a new point 
//		The original point is not modified
//
void	
C3x4Matrix::TransformTo( 
	const C3dCoord& in_pnt,
	C3dCoord*	io_pnt ) const
{
	io_pnt->X( (in_pnt.X() * m_i.X())
					+ (in_pnt.Y() * m_j.X())
					+ (in_pnt.Z() * m_k.X())
					+ m_t.X() );

	io_pnt->Y( (in_pnt.X() * m_i.Y())
					+ (in_pnt.Y() * m_j.Y())
					+ (in_pnt.Z() * m_k.Y())
					+ m_t.Y() );

	io_pnt->Z( (in_pnt.X() * m_i.Z())
					+ (in_pnt.Y() * m_j.Z())
					+ (in_pnt.Z() * m_k.Z())
					+ m_t.Z() );
}

void	
C3x4Matrix::TransformTo( 
	const C2dCoord& in_pnt,
	C2dCoord*	io_pnt ) const
{
	io_pnt->X( (in_pnt.X() * m_i.X())
					+ (in_pnt.Y() * m_j.X())
					+ m_t.X() );

	io_pnt->Y( (in_pnt.X() * m_i.Y())
					+ (in_pnt.Y() * m_j.Y())
					+ m_t.Y() );
}

void	
C3x4Matrix::TransformTo( 
	const C3dVec& in_vec,
	C3dVec*	io_vec ) const
{
	io_vec->X( (in_vec.X() * m_i.X())
					+ (in_vec.Y() * m_j.X())
					+ (in_vec.Z() * m_k.X()) );

	io_vec->Y( (in_vec.X() * m_i.Y())
					+ (in_vec.Y() * m_j.Y())
					+ (in_vec.Z() * m_k.Y()) );

	io_vec->Z( (in_vec.X() * m_i.Z())
					+ (in_vec.Y() * m_j.Z())
					+ (in_vec.Z() * m_k.Z()) );
}


// =======================================================================
//		TransformTo
//
//		Transform the given matrix by this one... multiply the matrices,
//		creating a new matrix.  Neither originals are modified.
//
//		The transformations of the two matrices will occur in a distinct
//		order:
//
//		The current matrix will occur first
//		The passed-in matrix will occur second
//
void
C3x4Matrix::TransformTo( 
	const C3x4Matrix&	in_mat,
	C3x4Matrix*	io_mat ) const
{
	// ------------ I

	io_mat->m_i.X( (in_mat.m_i.X() * m_i.X())
						 + (in_mat.m_j.X() * m_i.Y())
						 + (in_mat.m_k.X() * m_i.Z()) );

	io_mat->m_i.Y( (in_mat.m_i.Y() * m_i.X())
						 + (in_mat.m_j.Y() * m_i.Y())
						 + (in_mat.m_k.Y() * m_i.Z()) );

	io_mat->m_i.Z( (in_mat.m_i.Z() * m_i.X())
						 + (in_mat.m_j.Z() * m_i.Y())
						 + (in_mat.m_k.Z() * m_i.Z()) );
	
	// ------------ J

	io_mat->m_j.X( (in_mat.m_i.X() * m_j.X())
						 + (in_mat.m_j.X() * m_j.Y())
						 + (in_mat.m_k.X() * m_j.Z()) );

	io_mat->m_j.Y( (in_mat.m_i.Y() * m_j.X())
						 + (in_mat.m_j.Y() * m_j.Y())
						 + (in_mat.m_k.Y() * m_j.Z()) );

	io_mat->m_j.Z( (in_mat.m_i.Z() * m_j.X())
						 + (in_mat.m_j.Z() * m_j.Y())
						 + (in_mat.m_k.Z() * m_j.Z()) );

	// ------------ K

	io_mat->m_k.X( (in_mat.m_i.X() * m_k.X())
						 + (in_mat.m_j.X() * m_k.Y())
						 + (in_mat.m_k.X() * m_k.Z()) );

	io_mat->m_k.Y( (in_mat.m_i.Y() * m_k.X())
						 + (in_mat.m_j.Y() * m_k.Y())
						 + (in_mat.m_k.Y() * m_k.Z()) );

	io_mat->m_k.Z( (in_mat.m_i.Z() * m_k.X())
						 + (in_mat.m_j.Z() * m_k.Y())
						 + (in_mat.m_k.Z() * m_k.Z()) );

	// ------------ T

	io_mat->m_t.X( (in_mat.m_i.X() * m_t.X())
						 + (in_mat.m_j.X() * m_t.Y())
						 + (in_mat.m_k.X() * m_t.Z())
						 + in_mat.m_t.X() );

	io_mat->m_t.Y( (in_mat.m_i.Y() * m_t.X())
						 + (in_mat.m_j.Y() * m_t.Y())
						 + (in_mat.m_k.Y() * m_t.Z())
						 + in_mat.m_t.Y() );

	io_mat->m_t.Z( (in_mat.m_i.Z() * m_t.X())
						 + (in_mat.m_j.Z() * m_t.Y())
						 + (in_mat.m_k.Z() * m_t.Z())
						 + in_mat.m_t.Z() );
}

const C3x4Matrix&
C3x4Matrix::GlobalXform()
{
	return global_xform;
}

const C3x4Matrix&
C3x4Matrix::GlobalInverse()
{
	return global_xform;
}

