
package Weng.Math;

import Weng.System.*;

/**
 * A 2-dimensional bounding box representation.
 */
public class Box2d
{
	/**
	 * Creates an invalid 2d bounding box.
	 */
    public Box2d()
	{
	}

	/**
	 * Creates and initializes a 2d bounding box.
	 */
    public Box2d( double xmin, double ymin, double xmax, double ymax )
	{
		Set( xmin, ymin, xmax, ymax );
	}

	/**
	 * Sets the bounds of a 2d bounding box.
	 */
	public void Set( double xmin, double ymin, double xmax, double ymax )
	{
		m_xmin = xmin;
		m_ymin = ymin;
		m_xmax = xmax;
		m_ymax = ymax;
	}

	/**
	 * Gets the xmin of this 2d bounding box.
	 */
	public double Xmin()  { return m_xmin; }

	/**
	 * Gets the ymin of this 2d bounding box.
	 */
	public double Ymin()  { return m_ymin; }

	/**
	 * Gets the xmax of this 2d bounding box.
	 */
	public double Xmax()  { return m_xmax; }

	/**
	 * Gets the ymax of this 2d bounding box.
	 */
	public double Ymax()  { return m_ymax; }


	/**
	 * Gets the x dimension of this 2d bounding box.
	 */
	public double Dx()    { return (m_xmax - m_xmin); }

	/**
	 * Gets the y dimension of this 2d bounding box.
	 */
	public double Dy()    { return (m_ymax - m_ymin); }


	/**
	 * Sets the xmin of this 2d bounding box.
	 */
	public void Xmin( double xmin )  { m_xmin = xmin; }

	/**
	 * Sets the ymin of this 2d bounding box.
	 */
	public void Ymin( double ymin )  { m_ymin = ymin; }

	/**
	 * Sets the xmax of this 2d bounding box.
	 */
	public void Xmax( double xmax )  { m_xmax = xmax; }

	/**
	 * Sets the ymax of this 2d bounding box.
	 */
	public void Ymax( double ymax )  { m_ymax = ymax; }

	/**
	 * Determines whether this bounding box contains the given point.
	 * The box is logically expanded by the given tolerance.
	 */
	public boolean Contains( double x, double y, double tol )
	{
		if (x < (m_xmin - tol))
			return false;

		if (x > (m_xmax + tol))
			return false;

		if (y < (m_ymin - tol))
			return false;

		if (y > (m_ymax + tol))
			return false;

		return true;
	}

	/**
	 * Enlarges this bounding box to encompase the given bounding box.
	 */
	public void Union( Box2d box )
	{
		if (box.Xmin() < m_xmin)
			m_xmin = box.Xmin();

		if (box.Ymin() < m_ymin)
			m_ymin = box.Ymin();

		if (box.Xmax() > m_xmax)
			m_xmax = box.Xmax();

		if (box.Ymax() > m_ymax)
			m_ymax = box.Ymax();
	}

	/**
	 * Determines whether this bounding box overlaps the given bounding box
	 * within the given tolerance.
	 */
	public boolean Overlaps( Box2d box, double tol )
	{
		if (m_xmin > (box.Xmax()+tol))
			return false;

		if (m_xmax < (box.Xmin()-tol))
			return false;

		if (m_ymin > (box.Ymax()+tol))
			return false;

		if (m_ymax < (box.Ymin()-tol))
			return false;

		return true;
	}

	private double m_xmin =  Const.UNDEFINED;
	private double m_ymin =  Const.UNDEFINED;
	private double m_xmax = -Const.UNDEFINED;
	private double m_ymax = -Const.UNDEFINED;
}


 
 
 
 
 
 
 
 
 

