
package Weng.Math;

import Weng.System.Const;

/**
 * Use this class to create a 3-dimensional unit vector.
 */
public class UnitVec3d implements Triple
{
	/**
	 * Creates and initializes a unit vector having zero length.
	 */
	public UnitVec3d()
	{
		m_x = 0.0;
		m_y = 0.0;
		m_z = 0.0;
	}

	/**
	 * Creates and initializes a unit vector using the given
	 * vector components.  The components are normalized upon
	 * initialization.
	 */ 
	public UnitVec3d( double x, double y, double z )
	{
		Assign( x, y, z );
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
	 * Gets the Z component of this unit vector.
	 */
	public double Z() { return m_z; }

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
	 * Sets the Z component of this unit vector
	 * and then renormalizes the components.
	 */
	public void Z( double z )
	{
		m_z = z;
		Unitize();
	}

	/**
	 * In lieu of an assignment operator.
	 */
	public void Assign( UnitVec3d vec )
	{
		Assign( vec.m_x, vec.m_y, vec.m_z );
	}

	/**
	 * In lieu of an assignment operator.
	 */
	public void Assign( double x, double y, double z )
	{
		m_x = x;
		m_y = y;
		m_z = z;
		Unitize();
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
	 * Calculates a scaled 2d vector using the orientation of this vector.
	 */
	public void Scale( double dist, Vec3d vec )
	{
		vec.Assign( (m_x * dist), (m_y * dist), (m_z * dist) );
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

	/**
	 * Returns the cross product of the given vectors.
	 */
	static public void Cross( UnitVec3d vecA, UnitVec3d vecB, UnitVec3d result )
	{
		double x = vecA.Y() * vecB.Z() - vecA.Z() * vecB.Y();
		double y = vecA.Z() * vecB.X() - vecA.X() * vecB.Z();
		double z = vecA.X() * vecB.Y() - vecA.Y() * vecB.X();

		result.Assign( x, y, z );
	}

	private void Unitize()
	{
		double dSqrd = m_x * m_x + m_y * m_y + m_z * m_z;

		double arcLen = Math.sqrt( dSqrd );

		if (arcLen < Const.SMALL)
		{
			m_x = 0.0;
			m_y = 0.0;
			m_z = 0.0;
		}
		else
		{
			m_x = m_x / arcLen;
			m_y = m_y / arcLen;
			m_z = m_z / arcLen;
		}
	}

	private double m_x;
	private double m_y;
	private double m_z;
}
