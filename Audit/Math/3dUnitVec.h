#pragma once

class C3dVec;


class dllExport C3dUnitVec
{
public:

	C3dUnitVec();  // Use with caution!

	C3dUnitVec( double x, double y, double z );

	C3dUnitVec( const C3dUnitVec& vec );

	C3dUnitVec( double radians );

	void Init( double dx, double dy, double dz );

	// Assigment operator.
	const C3dUnitVec& operator = ( const C3dUnitVec& vec );

	// Index operator
	double operator [] ( int idx ) const		{ return (&m_x)[idx]; }

	// Conversion operator.
	operator C3dVec() const;

	virtual double X() const;
	virtual double Y() const;
	virtual double Z() const;

	virtual void X( double x );
	virtual void Y( double y );
	virtual void Z( double z );

	// subtraction
	C3dUnitVec operator - ( const C3dUnitVec & vec) const;

	// subtract
	C3dUnitVec& operator -= ( const C3dUnitVec &vec );

	// Scale.
	C3dVec operator * ( double scalar ) const;

	// dot product
	double operator * ( const C3dUnitVec &vec ) const;

	// cross product v = v0 ^ v1
	C3dUnitVec operator ^ ( const C3dUnitVec &vec ) const;

	void Reverse();

	virtual ~C3dUnitVec();

private:
	
	// Disabled.
	int operator == ( const C3dUnitVec& vec ) const;
	int operator != ( const C3dUnitVec& vec ) const;

private:

	void Unitize();
	void Adjust();

	double m_x;
	double m_y;
	double m_z;
};
