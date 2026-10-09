
package Weng.Math;

import Weng.System.Const;

/**
 * Use this class to create a 2-dimensional unit vector.
 */
public class UnitVec2d implements Tuple
{
	/**
	 * Creates and initializes a unit vector having zero length.
	 */
	public UnitVec2d()
	{
		m_x = 0.0;
		m_y = 0.0;
	}

	/**
	 * Creates and initializes a unit vector using the given
	 * vector components.  The components are normalized upon
	 * initialization.
	 */ 
	public UnitVec2d( double x, double y )
	{
		Assign( x, y );
	}

	/**
	 * Creates and initializes a unit vector using the given
	 * radian direction.
	 */
	public UnitVec2d( double radians )
	{
		Assign( Math.cos( radians ), Math.sin( radians ) );
	}

	/**
	 * Gets the X component of this unit vector.
	 */
	public double X() { return m_x; }

	/**
	 * Gets the Y component of this unit vector.
	 */
	public double Y() { return m_y; }

	/**
	 * Sets the X component of this unit vector
	 * and then renormalizes the components.
	 */
	public void X( double x )
	{
		m_x = x;
		Unitize();
	}

	/**
	 * Sets the Y component of this unit vector
	 * and then renormalizes the components.
	 */
	public void Y( double y )
	{
		m_y = y;
		Unitize();
	}

	/**
	 * In lieu of an assignment operator.
	 */
	public void Assign( UnitVec2d vec )
	{
		Assign( vec.m_x, vec.m_y );
	}

	/**
	 * In lieu of an assignment operator.
	 */
	public void Assign( double x, double y )
	{
		m_x = x;
		m_y = y;
		Unitize();
	}

	/**
	 * In lieu of an assignment operator.
	 */
	public void Assign( double radians )
	{
		Assign( Math.cos( radians ), Math.sin( radians ) );
	}

	/**
	 * Gets the XY planar direction of this vector in radians.
	 * @return  ( 0 <= result < TWOPI )
	 */
	public double Radians()
	{
		double dir;

		if (Math.abs( m_x ) < Const.SMALL)
		{
			dir = ((m_y < 0) ? Const.THREEHALFPI : Const.HALFPI);
		}
		else if (Math.abs( m_y ) < Const.SMALL)
		{
			dir = ((m_x < 0) ? Const.PI : 0.0);
		}
		else
		{
			dir = Math.atan( m_y/m_x );

			if (m_x < 0)
				dir += Const.PI;

			if (dir < 0)
				dir += Const.TWOPI;
		}

		return dir;
	}

	/**
	 * Calculates a vector that is rotated from this vector by the given amount.
	 */
	public void Rotate( double radians, UnitVec2d vec )
	{
		vec.Assign( Radians() + radians );
	}

	/**
	 * Calculates a scaled 2d vector using the orientation of this vector.
	 */
	public void Scale( double dist, Vec2d vec )
	{
		vec.Assign( (m_x * dist), (m_y * dist) );
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
	static public double Dot( UnitVec2d vecA, UnitVec2d vecB )
	{
		return (vecA.X() * vecB.X() + vecA.Y() * vecB.Y());
	}

	/**
	 * Returns the cross product of the given vectors.
	 */
	static public double Cross( UnitVec2d vecA, UnitVec2d vecB )
	{
		return (vecA.X() * vecB.Y() - vecA.Y() * vecB.X());
	}

	private void Unitize()
	{
		double dSqrd = m_x * m_x + m_y * m_y;

		double arcLen = Math.sqrt( dSqrd );

		if (arcLen < Const.SMALL)
		{
			m_x = 0.0;
			m_y = 0.0;
		}
		else
		{
			m_x = m_x / arcLen;
			m_y = m_y / arcLen;
		}
	}

	private double m_x;
	private double m_y;
}
