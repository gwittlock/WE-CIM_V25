#pragma once

class C2dVec;
class C2dUnitVec;
class C3dCoord;

class dllExport C3dVec
{
public:

	C3dVec();

	C3dVec( const C3dVec& vec );

	C3dVec( double x, double y, double z );
	
	void Init( double dx, double dy, double dz );

	// Assignment operator.
	const C3dVec& operator = ( const C3dVec& vec );

	// Index operator
	double operator [] ( int idx ) const	{ return (&m_x)[idx]; }

	// Conversion operators.
	operator C3dCoord() const;
	operator C2dVec() const;
	operator C2dUnitVec() const;

	double inline X() const { return m_x; }
	double inline Y() const { return m_y; }
	double inline Z() const { return m_z; }

	void inline X( double x ) { m_x = x; }
	void inline Y( double y ) { m_y = y; }
	void inline Z( double z ) { m_z = z; }

	double Length() const;

	// subtraction
	C3dVec operator - ( const C3dVec & vec) const;

	// subtract
	C3dVec& operator -= ( const C3dVec &vec );

	// add
	C3dVec operator + ( const C3dVec & vec ) const;

	// accumulate
	C3dVec & operator += ( const C3dVec &vec );

	// multiply by scalar 
	C3dVec operator * ( double scalar ) const;

	// dot product
	double operator * ( const C3dVec &vec ) const;

	// cross product v = v0 ^ v1
	C3dVec operator ^ ( const C3dVec &vec ) const;

	virtual ~C3dVec();

private:

	// Disabled.
	int operator == ( const C3dVec& vec ) const;
	int operator != ( const C3dVec& vec ) const;

private:

	double	m_x;
	double	m_y;
	double	m_z;
};
