
package Weng.Modeler;

import Weng.System.Portal;
import Weng.System.Const;
import Weng.Geometry.Point;
import Weng.Geometry.Arc;


/**
 * Use this class to create, query and manipulate an arc
 * entity that resides in the modeler's database.
 * <p>
 * With regards to <b>arc direction</b>, it suggested that you
 * use the constants Const.CCW and Const.CW that are
 * defined in Weng.System.Const.  Otherwise, arc direction
 * follows protractor convention (+) CCW / (-) CW.
 * <p>
 * With regards to <b>offset direction</b>, it suggested that you
 * use the constants Const.LEFT and Const.RIGHT that are
 * define in Weng.System.Const.  Otherwise, offset direction
 * follows protractor convention (+) LEFT / (-) RIGHT.  For
 * the uniformed, offset direction is relative to arc direction.
 */
public class DbArc extends DbCurve
{
	/**
	 * Creates an arc entity using the given ordinates.
	 */
	public DbArc( double xs, double ys, double zs,
				  double xe, double ye, double ze,
				  double xc, double yc, double zc,
				  int dir )
	{
		int id = Execute( 0, xs, ys, zs, xe, ye, ze, xc, yc, zc, dir );
		Id( id );
	}

	/**
	 * Creates an arc entity using the given points.
	 * @param ps the start point of the arc
	 * @param pe the end point of the arc
	 * @param pc the center point of the arc
	 * @param dir the arc direction (+) ccw / (-) cw
	 */
	public DbArc( Point ps, Point pe, Point pc, int dir )
	{
		int id = Execute( 0, ps.X(), ps.Y(), ps.Z(),
							 pe.X(), pe.Y(), pe.Z(),
							 pc.X(), pc.Y(), pc.Z(), dir );
		Id( id );
	}

	/**
	 * Creates an arc entity as a full circle having the given properties.
	 * @param pc the arc center point
	 * @param radius the arc radius
	 * @param dir the arc direction (+) ccw / (-) cw
	 */
	public DbArc( Point pc, double radius, int dir )
	{
		Arc arc = new Arc( pc, radius, dir );
		ConstructFromArc( arc );
	}

	/**
	 * Creates an arc entity having the given properties.
	 * @param pc the arc center point
	 * @param radius the arc radius
	 * @param as the starting angle in degrees
	 * @param ae the ending angle in degrees
	 * @param dir the arc direction (+) ccw / (-) cw
	 */
	public DbArc( Point pc, double radius, double as, double ae, int dir )
	{
		Arc arc = new Arc( pc, radius, as, ae, dir );
		ConstructFromArc( arc );
	}

	/**
	 * Creates an arc entity using the properties of the
	 * given geometric arc.
	 * @param arc the geometric arc entity whose properties should
	 *            be used to construct this arc entity
	 */
	public DbArc( Arc arc )
	{
		ConstructFromArc( arc );
	}

	/**
	 * Copy constructor.  Creates a unique copy of the given arc entity.
	 * @param dbArc the existing arc entity to be copied
	 */
	public DbArc( DbArc dbArc )
	{
		Point ps = dbArc.StartPt();
		Point pe = dbArc.EndPt();
		Point pc = dbArc.CenterPt();
		int  dir = dbArc.Dir();

		if (Model.IsRightHanded() == false)
			dir = -dir;

		int id = Execute( 0, ps.X(), ps.Y(), ps.Z(),
							 pe.X(), pe.Y(), pe.Z(),
							 pc.X(), pc.Y(), pc.Z(), dir );
		Id( id );
	}

	/**
	 * Conversion operator.
	 */
	static public DbArc DbArc( DbEntity dbEntity )
	{
		int id = dbEntity.Id();
		if (id <= 0)
			return null;

		if (dbEntity.Type() != Const.ARC)
			return null;

		return (new DbArc( id ));
	}

	/**
	 * Gets the center point of this arc entity as a geometric entity.
	 */
	public Point CenterPt()
	{
		String cmd = "Create:Extract: id=" + Id();
		boolean status = Portal.Execute( cmd );
		double xc = Portal.DoubleGet( "cx" );
		double yc = Portal.DoubleGet( "cy" );
		double zc = Portal.DoubleGet( "cz" );

		return (new Point( xc, yc, zc ));
	}

	/**
	 * Gets the direction of this arc entity.
	 * @return Const.CCW / Const.CW
	 */
	public int Dir()
	{
		int dir = 0;

		String cmd = "Solve:Dir:id=" + Id();
		if ( Portal.Execute( cmd ) )
		{
			dir = Portal.IntGet( "dir" );
			dir = ((dir < 0) ? Const.CW : Const.CCW);
		}

		return dir;
	}

	/**
	 * Gets the radius of this arc entity.
	 */
	public double Radius()
	{
		double radius = 0.0;

		String cmd = "Solve:Radius:id=" + Id();
		if ( Portal.Execute( cmd ) )
			radius = Portal.DoubleGet( "rad" );

		return radius;
	}

	/**
	 * Creates an arc entity by offsetting from this arc entity.
	 * @param offsetDirection (+) Const.LEFT / (-) Const.RIGHT
	 * @param offsetDistance an unsigned distance
	 * @return (null) When the arc can not be offset.  This usually
	 *         happens when you offset to the inside of the arc by
	 *         a distance that is greater than this arcs radius.
	 */
	public DbArc Offset( int offsetDirection, double offsetDistance )
	{
		DbArc dbArc = null;

		DbCurve dbCurve = Offset( offsetDistance, offsetDirection );

		if (dbCurve != null)
			dbArc = new DbArc( dbCurve.Id() );

		return dbArc;
	}

	/**
	 * Gets the center point of this arc entity as a point entity.
	 */
	public DbPoint DbCenterPt()
	{
		DbPoint dbPoint = null;

		if (Type() == Const.ARC)
		{
			String cmd = "Solve:CenterPt:id=" + Id();

			if ( Portal.Execute( cmd ) )
			{
				int id = Portal.IntGet( "id" );
				if (id > 0)
					dbPoint = new DbPoint( id );
			}
		}

		return dbPoint;
	}

	/**
	 * Inverts the arc. NOTE: The result is neither the arc-complement
	 * nor the same arc having opposite direction. The result is the
	 * having opposite direction and its center point mirrored across
	 * the arc chord.
	 */
	public void Invert()
	{
		if (Type() == Const.ARC)
		{
			String cmd = "Solve:Invert:id=" + Id();
			Portal.Execute( cmd );
		}
	}
	
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Protected methods.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	static protected int Execute( int id, Point ps, Point pe, Point pc, int dir )
	{
		return (DbArc.Execute( id, ps.X(), ps.Y(), ps.Z(),
								   pe.X(), pe.Y(), pe.Z(),
								   pc.X(), pc.Y(), pc.Z(), dir ));
	}

	protected DbArc( int id )
	{
		Id( id );
	}


	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Private methods.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	private void ConstructFromArc( Arc arc )
	{
		Point ps = arc.StartPt();
		Point pe = arc.EndPt();
		Point pc = arc.CenterPt();
		int dir = arc.Dir();

		int id = Execute( 0, ps.X(), ps.Y(), ps.Z(),
							 pe.X(), pe.Y(), pe.Z(),
							 pc.X(), pc.Y(), pc.Z(), dir );

		Id( id );
	}

	static private int Execute(
							int id,
							double xs, double ys, double zs,
							double xe, double ye, double ze,
							double xc, double yc, double zc,
							int dir )
	{
		String cmd = "Create:Arc:"
					 + " id=" + id
					 + ",sx=" + xs
					 + ",sy=" + ys
					 + ",sz=" + zs
					 + ",ex=" + xe
					 + ",ey=" + ye
					 + ",ez=" + ze
					 + ",cx=" + xc
					 + ",cy=" + yc
					 + ",cz=" + zc
					 + ",dir=" + dir;

		boolean okay = Portal.Execute( cmd );

		return (okay ? Portal.IntGet( "id" ) : 0);
	}
}

