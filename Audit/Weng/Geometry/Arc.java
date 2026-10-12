
package Weng.Geometry;

import Weng.System.Const;
import Weng.Math.*;

import Weng.System.*;


/**
 * Use this class to create an object representing a geometric arc.
 * This class does <b>not</b> create an arc entity in the modeler,
 * but is intended to be used for performing geometric calculations.
 */
public class Arc implements Elem
{
	public Arc( double xs, double ys, double zs,
				double xe, double ye, double ze,
				double xc, double yc, double zc,
				int dir )
	{
		Assign( xs, ys, zs, xe, ye, ze, xc, yc, zc, dir );
	}

	public Arc( Point ps, Point pe, Point pc, int dir )
	{
		Assign( ps, pe, pc, dir );
	}

	public Arc( Point pc, double radius, int dir )
	{
		m_ps.Assign( pc.X()+radius, pc.Y(), pc.Z() );
		m_pe.Assign( m_ps );
		m_pc.Assign( pc );
		m_dir = ((dir < 0) ? Const.CW : Const.CCW);
	}

	public Arc( Point pc, double radius, double as, double ae, int dir )
	{
		double x, y, radians;

		radians = as * Const.DEG2RAD;
		x = pc.X() + (radius * Math.cos(radians));
		y = pc.Y() + (radius * Math.sin(radians));
		m_ps.Assign( x, y, pc.Z() );

		radians = ae * Const.DEG2RAD;
		x = pc.X() + (radius * Math.cos(radians));
		y = pc.Y() + (radius * Math.sin(radians));
		m_pe.Assign( x, y, pc.Z() );

		m_pc.Assign( pc );

		m_dir = ((dir < 0) ? Const.CW : Const.CCW);
	}

	/**
	 * Copy constructor.
	 */
	public Arc( Arc arc )
	{
		Assign( arc );
	}

	/**
	 * Gets the start point of this arc.
	 */
	public Point StartPt()   { return m_ps; }

	/**
	 * Gets the end point of this arc.
	 */
	public Point EndPt()     { return m_pe; }

	/**
	 * Gets the center point of this arc.
	 */
	public Point CenterPt()  { return m_pc; }

	/**
	 * Gets the radius of this arc.
	 */
	public double Radius()
	{
		Vec3d vec = new Vec3d( m_ps, m_pc );
		double radius = vec.Length();
		return radius;
	}

	/**
	 * Gets the direction of this arc.
	 */
	public int Dir() { return m_dir; }

	/**
	 * Gets the tangent at the start of this arc.
	 */
	public UnitVec2d StartTan()
	{
		UnitVec2d startTan = TanAtPt( m_ps.X(), m_ps.Y() );
		return startTan;
	}

	/**
	 * Gets the tangent at the end of this arc.
	 */
	public UnitVec2d EndTan()
	{
		UnitVec2d startTan = TanAtPt( m_pe.X(), m_pe.Y() );
		return startTan;
	}

	/**
	 * Gets the starting and ending angles of this arc in radians
	 * @param angles [0] start angle / [1] end angle, where each angle
	 *               is in ( 0 <= result < TWOPI)
	 */
	public void Angles( double [] angles )
	{
		double as = Angle( m_ps.X(), m_ps.Y() );
		double ae = Angle( m_pe.X(), m_pe.Y() );

		if (m_dir < 0)
		{
			if (ae > as)
				//Need to check for wheather we are within 360 given CW
				as += Const.TWOPI;
		}
		else
		{
			if (ae < as)
				//Need to check for wheather we are within 360 given ccw
				ae += Const.TWOPI;
		}

		if (Math.abs( ae-as ) < Const.SMALL)
		{
			// ASSUMPTION: zero-length arcs are not allowed.

			if (m_dir < 0)
				as += Const.TWOPI;
			else
				ae += Const.TWOPI;
		}

		angles[0] = as;
		angles[1] = ae;
	}

	/**
	 * Gets the unsigned included angle of this arc (in radians)
	 */
	public double IncludedAngle()
	{
		double [] angles = new double[2];
		Angles( angles );
		return ( Math.abs( angles[1]-angles[0] ) );
	}

	/**
	 * Gets the arc length of this arc
	 */
	public double ArcLen()
	{
		double radians = IncludedAngle();
		double arcLen = (radians / Const.TWOPI) * Radius();
		return arcLen;
	}

	/**
	 * Gets the point at the given u-parameter on this arc.
	 */
	public Point Point( double uparam )
	{
		double [] angles = new double[2];
		Angles( angles );

		double radius = Radius();
		double radians = angles[0] + (uparam * (angles[1] - angles[0]));

		double x = m_pc.X() + (radius * Math.cos( radians ));
		double y = m_pc.Y() + (radius * Math.sin( radians ));
		double z = m_pc.Z();

		return (new Point( x, y, z ) );
	}

	/**
	 * In lieu of assignment operators
	 */
	public boolean Assign( double xs, double ys, double zs,
						   double xe, double ye, double ze,
						   double xc, double yc, double zc,
						   int dir )
	{
		m_ps.Assign( xs, ys, zs );
		m_pe.Assign( xe, ye, ze );
		m_pc.Assign( xc, yc, zc );
		m_dir = ((dir < 0) ? Const.CW : Const.CCW);

		return true;  // should check integrity
	}

	/**
	 * In lieu of assignment operators
	 */
	public boolean Assign( Point ps, Point pe, Point pc, int dir )
	{
		m_ps.Assign( ps );
		m_pe.Assign( pe );
		m_pc.Assign( pc );
		m_dir = ((dir < 0) ? Const.CW : Const.CCW);

		return true;  // should check integrity
	}

	/**
	 * In lieu of assignment operators
	 */
	public boolean Assign( Arc arc )
	{
		m_ps.Assign( arc.StartPt() );
		m_pe.Assign( arc.EndPt() );
		m_pc.Assign( arc.CenterPt() );
		m_dir = arc.m_dir;

		return true;  // should check integrity
	}


	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Private methods & data.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	private UnitVec2d TanAtPt( double x, double y )
	{
		double dx;
		double dy;

		// At first implementation, assume XY arc.
		if (m_dir > 0)
		{
			dx = y - m_pc.Y();
			dy = m_pc.X() - x;
		}
		else
		{
			dx = m_pc.Y() - y;
			dy = x - m_pc.X();
		}

		UnitVec2d startTan = new UnitVec2d( dx, dy );

		return startTan;
	}

	// Result in radians where ( 0 <= result < Const.TWOPI).
	private double Angle( double x, double y )
	{
		double dx = x - m_pc.X();
		double dy = y - m_pc.Y();

		UnitVec2d vec = new UnitVec2d( dx, dy );

		double angle = vec.Radians();

		return angle;
	}

	protected void finalize() throws Throwable
	{
		m_ps = null;
		m_pe = null;
		m_pc = null;

		super.finalize();
	}

	private Point m_ps = new Point();
	private Point m_pe = new Point();
	private Point m_pc = new Point();
	private int m_dir;
}
