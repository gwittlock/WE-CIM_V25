
package Weng.Geometry;

import Weng.Math.*;

/**
 * As an interface, you can not instantiate an instance of this class.
 */
public interface Elem
{
	/**
	 * Gets the start point of this geometric entity.
	 */
	public Point StartPt();

	/**
	 * Gets the end point of this geometric entity.
	 */
	public Point EndPt();

	/**
	 * Gets the tangent vector at the start of this geometric entity.
	 */
	public UnitVec2d StartTan();

	/**
	 * Gets the tangent vector at the end of this geometric entity.
	 */
	public UnitVec2d EndTan();

	/**
	 * Gets the arc length of this geometric entity.
	 */
	public double ArcLen();

	/**
	 * Gets the point at the given u-parameter on this entity.
	 */
	public Point Point( double uparam );
}

