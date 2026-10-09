
package Weng.Math;

import Weng.System.Const;
import Weng.Geometry.Point;


/**
 * Use this class to create a 3-dimensional vector.
 */
public class Vec3d implements Triple
{
	/**
	 * Creates and initializes a vector having zero length.
	 */
	public Vec3d()
	{
		Assign( 0.0, 0.0, 0.0 );
	}

	/**
	 * Creates and initializes a vector using the given components.
	 */
	public Vec3d( double dx, double dy, double dz )
	{
		Assign( dx, dy, dz );
	}

	/**
	 * Creates and initializes a vector as the difference between two points.
	 */
	public Vec3d( Point ps, Point pe )
	{
		Assign( pe.X()-ps.X(), pe.Y()-ps.Y(), pe.Z()-ps.Z() );
	}

	/**
	 * Copy constructor
	 */
	public Vec3d( Vec3d vec )
	{
		Assign( vec );
	}

	/**
	 * Gets the X component of this vector.
	 */
	public double X() { return m_x; }

	/**
	 * Gets the Y component of this vector.
	 */
	public double Y() { return m_y; }

	/**
	 * Gets the Z component of this vector.
	 */
	public double Z() { return m_z; }

	/**
	 * Sets the X component of this vector.
	 */
	public void X( double x )  { m_x = x; }

	/**
	 * Sets the Y component of this vector.
	 */
	public void Y( double y )  { m_y = y; }

	/**
	 * Sets the Z component of this vector.
	 */
	public void Z( double z )  { m_z = z; }

	/**
	 * Gets the length of this vector.
	 */
	public double Length()
	{
		double len = Math.sqrt( m_x*m_x + m_y*m_y + m_z*m_z );
		return len;
	}

	/**
	 * In lieu of an assignment operator.
	 */
	public void Assign( Vec3d vec )
	{
		m_x = vec.m_x;
		m_y = vec.m_y;
		m_z = vec.m_z;
	}

	/**
	 * In lieu of an assignment operator.
	 */
	public void Assign( double x, double y, double z )
	{
		m_x = x;
		m_y = y;
		m_z = z;
	}

	/**
	 * Flips the direction of this vector.
	 */
	public void Reverse()
	{
		m_x = -m_x;
		m_y = -m_y;
		m_z = -m_z;
	}


	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Private methods & data.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	double m_x;
	double m_y;
	double m_z;
}
