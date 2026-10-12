
package Weng.CodeGen;

import Weng.System.Msg;
import Weng.Math.Box2d;
import Weng.Geometry.Point;
import Weng.Geometry.Line;
import Weng.Geometry.ClosestData;


/**
 *
 */
public class SafeZone
{
	/**
	 * Creates an invalid SafeZone object.
	 */
	public SafeZone()
	{
		m_box = new Box2d();
	}

	/**
	 * Sets the properties of a SafeZone object (typically a clamp zone).
	 * @param x x location of clamp
	 * @param y y location of clamp
	 * @param xmin xmin of clamp bounding box
	 * @param ymin ymin of clamp bounding box
	 * @param xmax xmax of clamp bounding box
	 * @param ymax ymax of clamp bounding box
	 */
	public void Set( double x, double y, double xmin, double ymin, double xmax, double ymax )
	{
		m_x = x;
		m_y = y;
		m_box.Set( xmin, ymin, xmax, ymax );
	}

	/**
	 * Determines with the line from ps to pe intersects this safe zone.
	 * @param ps the starting location of a move
	 * @param pe the ending location of a move
	 * @param dist a bounding box delta that is used to minimize computation.
	 *    If the bounding box of the move and this safe zone are within the
	 *    given distance, then the move is intersected with the bounding box
	 *    of this safe zone.
	 */
	public boolean Collision( Point ps, Point pe, double dist )
	{
		double xmin, ymin, xmax, ymax;
		boolean collision = false;

		if (ps.X() < pe.X())
		{
			xmin = ps.X();
			xmax = pe.X();
		}
		else
		{
			xmin = pe.X();
			xmax = ps.X();
		}

		if (ps.Y() < pe.Y())
		{
			ymin = ps.Y();
			ymax = pe.Y();
		}
		else
		{
			ymin = pe.Y();
			ymax = ps.Y();
		}

		Box2d move = new Box2d( xmin, ymin, xmax, ymax );
		if ( m_box.Overlaps( move, dist ) )
		{
			ClosestData data;
			Point pt = new Point();

			Line line = new Line( ps, pe );

			int tally = 0;

			pt.Assign( m_box.Xmin(), m_box.Ymin(), 0. );
			data = line.PointClosest( pt );
			tally += data.Side();

			pt.Assign( m_box.Xmin(), m_box.Ymax(), 0. );
			data = line.PointClosest( pt );
			tally += data.Side();

			pt.Assign( m_box.Xmax(), m_box.Ymax(), 0. );
			data = line.PointClosest( pt );
			tally += data.Side();

			pt.Assign( m_box.Xmax(), m_box.Ymin(), 0. );
			data = line.PointClosest( pt );
			tally += data.Side();

			collision = (Math.abs( tally ) != 4);
		}

		return collision;
	}

	/**
	 * Gets the x location of this safe zone (typically the x location of a clamp)
	 */
	public double X()		{ return m_x; }

	/**
	 * Gets the x location of this safe zone (typically the y location of a clamp)
	 */
	public double Y()		{ return m_y; }

	/**
	 * Gets the xmin of the bounding box of this safe zone.
	 */
	public double Xmin()	{ return m_box.Xmin(); }

	/**
	 * Gets the ymin of the bounding box of this safe zone.
	 */
	public double Ymin()	{ return m_box.Ymin(); }

	/**
	 * Gets the xmax of the bounding box of this safe zone.
	 */
	public double Xmax()	{ return m_box.Xmax(); }

	/**
	 * Gets the ymax of the bounding box of this safe zone.
	 */
	public double Ymax()	{ return m_box.Ymax(); }

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// NOTE
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	//
	// Ideally, we should have a generic circumnavigation method.
	// The expedient solution for Whitney is much simpler, however.
	//
	// /**
	//  * Creates an array of points representing a path around this safe zone.
	//  * @param ps the start location
	//  * @param pe the end location
	//  * @param cw (true) clockwise navigation / (false) counter-clockwise navigation
	//  * @param dist the minimum allowed distance to the safe zone
	//  * @return an array of points representing a path around this safe zone [ps..pe]
	//  */
	// public Point[] Circumnavigate( Point ps, Point pe, boolean cw, double dist )
	//
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	private double m_x = 0.0;  // clamp location
	private double m_y = 0.0;  // clamp location

	private Box2d m_box = null;
}

