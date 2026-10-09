#pragma once

class C2dUnitVec;
class C3dVec;


class dllExport C2dVec 
{
public:

	// default constructor, members undefined
	C2dVec();

	C2dVec( const C2dVec& vec );

	// construct
	C2dVec( double x, double y );

	void Init( double dx, double dy );
	void Init( const C2dVec& vec );

	const C2dVec& operator = ( const C2dVec& vec );

	// Index operator
	double operator [] ( int idx ) const		{ return (&m_x)[idx]; }

	// Conversion operators.
	operator C2dUnitVec() const;
	operator C3dVec() const;

	double inline X() const { return m_x; }
	double inline Y() const { return m_y; }

	void inline X( double x ) { m_x = x; }
	void inline Y( double y ) { m_y = y; }

	double Length() const;

	// subtraction
	C2dVec operator - ( const C2dVec & vec) const;

	// subtract
	C2dVec& operator -= ( const C2dVec &vec );

	// add
	C2dVec operator + ( const C2dVec & vec ) const;

	// accumulate
	C2dVec& operator += ( const C2dVec &vec );

	// rotation
	C2dVec operator + ( double radians ) const;
	C2dVec& operator += ( double radians );
	C2dVec operator - ( double radians ) const;
	C2dVec& operator -= ( double radians );

	// multiply by scalar 
	C2dVec operator * ( double scalar ) const;

	C2dVec& operator *= ( double scalar );

	// dot product
	double operator * ( const C2dVec &vec ) const;
	
	// perp-dot
	double PerpDot( const C2dVec& vec ) const;

	// cross product v = v0 ^ v1
	double operator ^ ( const C2dVec &vec ) const;

	// Result in radians where ( 0 <= result < TWOPI).
	// Technically speaking, this should work, but if you want cleanliness,
	// use C2dUnitVec.  Only, that's much slower.
	double Radians() const;

	virtual ~C2dVec();

private:

	// Disabled.
	int operator == ( const C2dVec& vec ) const;
	int operator != ( const C2dVec& vec ) const;

private:

	double m_x;
	double m_y;
};
