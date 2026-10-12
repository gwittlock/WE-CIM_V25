
package Weng.Math;

import Weng.System.Const;
import Weng.Geometry.Point;


/**
 * Use this class to create a 2-dimensional vector.
 */
public class Vec2d implements Tuple
{
	/**
	 * Creates and initializes a vector having zero length.
	 */
	public Vec2d()
	{
		Assign( 0.0, 0.0 );
	}

	/**
	 * Creates and initializes a vector using the given components.
	 */
	public Vec2d( double dx, double dy )
	{
		Assign( dx, dy );
	}

	/**
	 * Creates and initializes a vector as the difference between two points.
	 */
	public Vec2d( Point ps, Point pe )
	{
		Assign( ps, pe );
	}

	/**
	 * Copy constructor
	 */
	public Vec2d( Vec2d vec )
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
	 * Sets the X component of this vector.
	 */
	public void X( double x )  { m_x = x; }

	/**
	 * Sets the Y component of this vector.
	 */
	public void Y( double y )  { m_y = y; }

	/**
	 * Gets the length of this vector.
	 */
	public double Length()
	{
		double len = Math.sqrt( m_x*m_x + m_y*m_y );
		return len;
	}

	/**
	 * In lieu of an assignment operator.
	 */
	public void Assign( Vec2d vec )
	{
		m_x = vec.m_x;
		m_y = vec.m_y;
	}

	/**
	 * In lieu of an assignment operator.
	 */
	public void Assign( Point ps, Point pe )
	{
		Assign( (pe.X() - ps.X()), (pe.Y() - ps.Y()) );
	}

	/**
	 * In lieu of an assignment operator.
	 */
	public void Assign( double x, double y )
	{
		m_x = x;
		m_y = y;
	}

	/**
	 * Gets the XY planar direction of this vector in radians.
	 * @return  ( 0 <= result < TWOPI )
	 */
	public double Radians()
	{
		UnitVec2d vec = new UnitVec2d( m_x, m_y );
		return ( vec.Radians() );
	}

	/**
	 * Calculates a vector that is rotated from this vector by the given amount.
	 */
	public void Rotate( double radians, Vec2d vec )
	{
		double len = Length();
		UnitVec2d tvec = new UnitVec2d( m_x, m_y );

		tvec.Rotate( radians, tvec );

		vec.Assign( (tvec.X() * len), (tvec.Y() * len) );
	}

	/**
	 * Flips the direction of this vector.
	 */
	public void Reverse()
	{
		m_x = -m_x;
		m_y = -m_y;
	}

	/**
	 * Returns the dot product of the given vectors.
	 */
	static public double Dot( Vec2d vecA, Vec2d vecB )
	{
		return (vecA.X() * vecB.X() + vecA.Y() * vecB.Y());
	}

	/**
	 * Returns the cross product of the given vectors.
	 */
	static public double Cross( Vec2d vecA, Vec2d vecB )
	{
		return (vecA.X() * vecB.Y() - vecA.Y() * vecB.X());
	}


	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Private methods & data.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	private double m_x;
	private double m_y;
}
