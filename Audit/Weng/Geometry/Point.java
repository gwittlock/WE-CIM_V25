
package Weng.Geometry;

import Weng.System.Const;
import Weng.Math.Triple;


/**
 * Use this class to create an object representing a geometric point.
 * This class does <b>not</b> create an point entity in the modeler,
 * but is intended to be used for performing geometric calculations.
 */
public class Point implements Triple
{
	/**
	 * The default constructor creates and initializes geometric point
	 * whose ordinate values are all Const.UNDEFINED
	 */
	public Point()
	{
		Assign( Const.UNDEFINED, Const.UNDEFINED, Const.UNDEFINED );
	}

	/**
	 * Creates and initializes geometric point with the given ordinate values.
	 */
	public Point( double x, double y, double z )
	{
		Assign( x, y, z );
	}

	/**
	 * Copy constructor.
	 */
	public Point( Point pt )
	{
		Assign( pt.X(), pt.Y(), pt.Z() );
	}

	/**
	 * Creates a point at an angle and distance relative
	 * to this point, where the angle is in degrees.
	 */
	public Point Relative( double angle, double dist )
	{
		double radians = angle * Const.DEG2RAD;
		double dx = dist * Math.cos( radians );
		double dy = dist * Math.sin( radians );

		return (new Point( m_x+dx, m_y+dy, m_z ));
	}

	/**
	 * Gets the X ordinate of this point
	 */
	public double X()  { return m_x; }

	/**
	 * Gets the Y ordinate of this point
	 */
	public double Y()  { return m_y; }

	/**
	 * Gets the Z ordinate of this point
	 */
	public double Z()  { return m_z; }

	/**
	 * Sets the X ordinate of this point
	 */
	public void X( double x )  { m_x = x; }

	/**
	 * Sets the Y ordinate of this point
	 */
	public void Y( double y )  { m_y = y; }

	/**
	 * Sets the Z ordinate of this point
	 */
	public void Z( double z )  { m_z = z; }

	/**
	 * Determines whether the given point is within tolerance
	 * of this point
	 * @param pt the point to be compared
	 * @param tol the tolerance with which to do the comparison
	 * @return (true) if this and the given point are within tolerance
	 */
	public boolean WithinTol( Point pt, double tol )
	{
		double dx = Math.abs( m_x - pt.m_x );

		if (dx > tol)
			return false;

		double dy = Math.abs( m_y - pt.m_y );

		if (dy > tol)
			return false;

		double dz = Math.abs( m_z - pt.m_z );

		if (dz > tol)
			return false;

		double magSqrd = dx * dx + dy * dy + dz * dz;
		double tolSqrd = tol * tol;

		return ((magSqrd <= tolSqrd) ? true : false);
	}

	/**
	 * In lieu of assignment operators
	 */
	public void Assign( double x, double y, double z )
	{
		m_x = x;
		m_y = y;
		m_z = z;
	}

	/**
	 * In lieu of assignment operators
	 */
	public void Assign( Point pt )
	{
		m_x = pt.X();
		m_y = pt.Y();
		m_z = pt.Z();
	}


	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Private methods & data.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	private double m_x;
	private double m_y;
	private double m_z;
}
