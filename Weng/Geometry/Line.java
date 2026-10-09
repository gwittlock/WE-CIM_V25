
package Weng.Geometry;

import Weng.System.Msg;
import Weng.System.Const;
import Weng.Math.*;


/**
 * Use this class to create an object representing a geometric line.
 * This class does <b>not</b> create an line entity in the modeler,
 * but is intended to be used for performing geometric calculations.
 */
public class Line implements Elem
{
	public Line( double xs, double ys, double zs,
				 double xe, double ye, double ze )
	{
		Assign( xs, ys, zs, xe, ye, ze );
	}

	public Line( Point ps, Point pe )
	{
		Assign( ps, pe );
	}

	public Line( Point ps, double angle, double length )
	{
		double radians = angle * Const.DEG2RAD;
		double xe = ps.X() + (length * Math.cos( radians ));
		double ye = ps.Y() + (length * Math.sin( radians ));
		double ze = ps.Z();

		Assign( ps.X(), ps.Y(), ps.Z(), xe, ye, ze );
	}

	public Line( Point ps, UnitVec3d vec, double length )
	{
		double xe = ps.X() + (length * vec.X());
		double ye = ps.Y() + (length * vec.Y());
		double ze = ps.Z() + (length * vec.Z());

		Assign( ps.X(), ps.Y(), ps.Z(), xe, ye, ze );
	}

	/**
	 * Copy constructor.
	 */
	public Line( Line line )
	{
		Assign( line );
	}

	/**
	 * Gets the start point of this line.
	 */
	public Point StartPt()   { return m_ps; }

	/**
	 * Gets the end point of this line.
	 */
	public Point EndPt()     { return m_pe; }

	/**
	 * Gets the tangent at the start of this line.
	 */
	public UnitVec2d StartTan()    { return Dir2d(); }

	/**
	 * Gets the tangent at the end of this line.
	 */
	public UnitVec2d EndTan()    { return Dir2d(); }

	/**
	 * Gets the arc length of this line
	 */
	public double ArcLen()
	{
		Vec3d vec = new Vec3d( m_pe, m_ps );

		double arcLen = vec.Length();

		return arcLen;
	}

	/**
	 * Gets the point at the given u-parameter on this line.
	 */
	public Point Point( double uparam )
	{
		UnitVec3d vec = Dir3d();

		double dist = ArcLen() * uparam;

		double x = m_ps.X() + (dist * vec.X());
		double y = m_ps.Y() + (dist * vec.Y());
		double z = m_ps.Z() + (dist * vec.Z());

		return ( new Point( x, y, z ) );
	}	

	/**
	 * In lieu of assignment operators
	 */
	public void Assign( double xs, double ys, double zs,
					    double xe, double ye, double ze )
	{
		m_ps.Assign( xs, ys, zs );
		m_pe.Assign( xe, ye, ze );
	}

	/**
	 * In lieu of assignment operators
	 */
	public void Assign( Point ps, Point pe )
	{
		m_ps.Assign( ps );
		m_pe.Assign( pe );
	}

	/**
	 * In lieu of assignment operators
	 */
	public void Assign( Line line )
	{
		m_ps.Assign( line.StartPt() );
		m_pe.Assign( line.EndPt() );
	}

	/**
	 * NOTE: 2d (xy) solution
	 */
	public ClosestData PointClosest( Point pt )
	{
		Point closest;
		Vec2d ref, vec;
		double xp, yp, dot, cross;
		double magSqrd, uparam, dist;
		int side;

		ref = new Vec2d( m_ps, m_pe );

		magSqrd = Vec2d.Dot( ref, ref );

		if (magSqrd < (Const.SMALL * Const.SMALL))
			return null;  // fails on zero-length lines.

		vec = new Vec2d( m_ps, pt );

		dot = Vec2d.Dot( ref, vec );

		uparam = dot / magSqrd;

		xp = m_ps.X() + (uparam * ref.X());
		yp = m_ps.Y() + (uparam * ref.Y());

		closest = new Point( xp, yp, 0. );

		vec.Assign( closest, pt );

		dist = vec.Length();
		cross = Vec2d.Cross( ref, vec );

		if (Math.abs( cross ) >= Const.VECTOR_SMALL)
			side = ((cross > 0) ? Const.LEFT : Const.RIGHT);
		else
			side = 0;
		
		return ( new ClosestData( closest, uparam, dist, side ) );
	}

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Private methods & data.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	private UnitVec2d Dir2d()
	{
		double dx = m_pe.X() - m_ps.X();
		double dy = m_pe.Y() - m_ps.Y();

		return ( new UnitVec2d( dx, dy ) );
	}

	private UnitVec3d Dir3d()
	{
		double dx = m_pe.X() - m_ps.X();
		double dy = m_pe.Y() - m_ps.Y();
		double dz = m_pe.Z() - m_ps.Z();

		return ( new UnitVec3d( dx, dy, dz ) );
	}

	protected void finalize() throws Throwable
	{
		m_ps = null;
		m_pe = null;

		super.finalize();
	}

	private Point m_ps = new Point();
	private Point m_pe = new Point();
}

