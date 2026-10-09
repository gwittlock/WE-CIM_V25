#pragma once

class C2dVec;


class dllExport C2dUnitVec
{
public:

	C2dUnitVec();  // Use with caution!

	C2dUnitVec( double x, double y );

	C2dUnitVec( const C2dUnitVec& vec );

	C2dUnitVec( double radians );

	bool IsValid() const;

	void Init( double dx, double dy );

	// Assigment operator.
	const C2dUnitVec& operator = ( const C2dUnitVec& vec );

	// Index operator
	double operator [] ( int idx ) const		{ return (&m_x)[idx]; }

	// Conversion operator.
	operator C2dVec() const;

	virtual double X() const;

	virtual double Y() const;

	virtual void X( double x );

	virtual void Y( double y );

	// Rotate.
	C2dUnitVec operator + ( double radians ) const;
	const C2dUnitVec& operator += ( double radians );
	C2dUnitVec operator - ( double radians ) const;
	const C2dUnitVec& operator -= ( double radians );

	// subtraction
	C2dUnitVec operator - ( const C2dUnitVec & vec) const;

	// subtract
	C2dUnitVec& operator -= ( const C2dUnitVec &vec );

	// Scale.
	C2dVec operator * ( double scalar ) const;

	// dot product
	double operator * ( const C2dUnitVec &vec ) const;

	// perp-dot
	double PerpDot( const C2dUnitVec& vec ) const;

	// Angle to a vector; 0..+-180
	double AngleTo( const C2dUnitVec& vec, bool winding ) const;

	// cross product v = v0 ^ v1
	double operator ^ ( const C2dUnitVec &vec ) const;

	// Result in radians where ( 0 <= result < TWOPI).
	double Radians() const;

	// Set this vectors' components.
	void Radians( double radians );

	virtual ~C2dUnitVec();

private:
	
	// Disabled.
	int operator == ( const C2dUnitVec& vec ) const;
	int operator != ( const C2dUnitVec& vec ) const;

private:

	void Unitize();
	void Adjust();

	double m_x;
	double m_y;
};
