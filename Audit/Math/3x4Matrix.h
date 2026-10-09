#pragma once

#include "3dCoord.h"
#include "3dVec.h"

// ==================================================================

class dllExport C3x4Matrix
{
public:

	C3x4Matrix();
	C3x4Matrix( const C3x4Matrix& matrix );
	C3x4Matrix( const C3dVec& in_i, const C3dVec& in_j, const C3dVec& in_k, const C3dCoord& in_t );

	C3x4Matrix( double ix, double iy, double iz,
				double jx, double jy, double jz,
				double kx, double ky, double kz,
				double tx, double ty, double tz );

	virtual ~C3x4Matrix();

	const C3x4Matrix& operator = ( const C3x4Matrix& );

	void	setI( const C3dVec& in_i )									{ m_i = in_i; }
	void	setI( double in_x, double in_y, double in_z )		{ m_i = C3dVec( in_x, in_y, in_z ); }
	const C3dVec&	getI( void ) const								{ return m_i; }

	void	setJ( const C3dVec& in_j )									{ m_j = in_j; }
	void	setJ( double in_x, double in_y, double in_z )		{ m_j = C3dVec( in_x, in_y, in_z ); }
	const C3dVec&	getJ( void ) const								{ return m_j; }

	void	setK( const C3dVec& in_k )									{ m_k = in_k; }
	void	setK( double in_x, double in_y, double in_z )		{ m_k = C3dVec( in_x, in_y, in_z ); }
	const C3dVec&	getK( void ) const 								{ return m_k; }

	void	setT( const C3dCoord& in_t )								{ m_t = in_t; }
	void	setT( double in_x, double in_y, double in_z )		{ m_t = C3dCoord( in_x, in_y, in_z ); }
	const C3dCoord&	getT( void ) const							{ return m_t; }

	// Angles are in radians
	void	setUnit( void );
	void	setXYAngle( double in_angle );
	void	setXZAngle( double in_angle );
	void	setYZAngle( double in_angle );

	void	Scale( double in_scale );
	void	Scale( double in_scale_x, double in_scale_y, double in_scale_z );
	void	Shift( const C3dVec&	in_shift );

	void	Transform( C3dCoord* io_pnt ) const;
	void	Transform( C2dCoord* io_pnt ) const;
	void	Transform( C3dVec* io_vec ) const;
	void	Transform( C2dVec* io_vec ) const;
	void	Transform( C3x4Matrix* io_mat ) const;

	void	InvertTo( C3x4Matrix* io_mat ) const;
	void	TransformTo( const C3dCoord& in_pnt, C3dCoord* io_pnt ) const;
	void	TransformTo( const C2dCoord& in_pnt, C2dCoord* io_pnt ) const;
	void	TransformTo( const C3dVec& in_pnt, C3dVec* io_pnt ) const;
	void	TransformTo( const C3x4Matrix& in_mat, C3x4Matrix* io_mat ) const;

public:

	static const C3x4Matrix& GlobalXform();
	static const C3x4Matrix& GlobalInverse();

protected:

private:
	// Disabled.
	int operator == ( const C3x4Matrix& ) const;
	int operator != ( const C3x4Matrix& ) const;

private:
	C3dVec	m_i;
	C3dVec	m_j;
	C3dVec	m_k;
	C3dCoord	m_t;
};
