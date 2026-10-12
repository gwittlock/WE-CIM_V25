
package Weng.Modeler;

import Weng.System.Portal;
import Weng.System.Const;
import Weng.Geometry.Point;
import Weng.Geometry.Line;
import Weng.Math.UnitVec2d;
import Weng.Math.UnitVec3d;


/**
 * Use this class to create, query and manipulate a line
 * entity that resides in the modeler's database.
 */
public class DbLine extends DbCurve
{
	/**
	 * Creates a line entity using the given ordinates.
	 */
	public DbLine( double xs, double ys, double zs, double xe, double ye, double ze )
	{
		int id = Execute( 0, xs, ys, zs, xe, ye, ze );
		Id( id );
	}

	/**
	 * Creates a line entity between the given geometric points.
	 */
	public DbLine( Point ps, Point pe )
	{
		int id = Execute( 0, ps, pe );
		Id( id );
	}

	/**
	 * Creates a line entity starting at the given point and having
	 * the given direction and length.
	 * @param ps the start point of the line
	 * @param angle the direction of the line (degrees)
	 * @param length the length of the line
	 */
	public DbLine( Point ps, double angle, double length )
	{
		Line line = new Line( ps, angle, length );
		ConstructFromLine( line );
	}

	/**
	 * Creates a line entity starting at the given point and having
	 * the given direction and length.
	 * @param ps the start point of the line
	 * @param angle the direction of the line
	 * @param length the length of the line
	 */
	public DbLine( Point ps, UnitVec3d vec, double length )
	{
		Line line = new Line( ps, vec, length );
		ConstructFromLine( line );
	}

	/**
	 * Creates an line entity using the properties of the
	 * given geometric line.
	 * @param line the geometric line entity whose properties should
	 *             be used to construct this line entity
	 */
	public DbLine( Line line )
	{
		ConstructFromLine( line );
	}

	/**
	 * Copy constructor.  Creates a unique copy of the given line entity.
	 * @param dbline the existing line entity to be copied
	 */
	public DbLine( DbLine dbLine )
	{
		Point ps = dbLine.StartPt();
		Point pe = dbLine.EndPt();
		int id = Execute( 0, ps.X(), ps.Y(), ps.Z(), pe.X(), pe.Y(), pe.Z() );
		Id( id );
	}

	/**
	 * Conversion operator.
	 */
	static public DbLine DbLine( DbEntity dbEntity )
	{
		int id = dbEntity.Id();
		if (id <= 0)
			return null;

		if (dbEntity.Type() != Const.LINE)
			return null;
			
		return (new DbLine( id ));
	}

	/**
	 * Creates an line entity by offsetting from this line entity.
	 * @param offsetDirection (+) Const.LEFT / (-) Const.RIGHT
	 * @param offsetDistance an unsigned distance
	 * @return (null) indicates failure.
	 */
	public DbLine Offset( int offsetDirection, double offsetDistance )
	{
		DbLine dbLine = null;

		DbCurve dbCurve = Offset( offsetDistance, offsetDirection );

		if (dbCurve != null)
			dbLine = new DbLine( dbCurve.Id() );

		return dbLine;
	}


	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Protected methods.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	protected DbLine()
	{
		Id(0);
	}


	static protected int Execute( int id, Point ps, Point pe )
	{
		return (DbLine.Execute( id, ps.X(), ps.Y(), ps.Z(), pe.X(), pe.Y(), pe.Z() ));
	}

	protected DbLine( int id )
	{
		Id( id );
	}


	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Private methods.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	private void ConstructFromLine( Line line )
	{
		Point ps = line.StartPt();
		Point pe = line.EndPt();
		int id = Execute( 0, ps.X(), ps.Y(), ps.Z(), pe.X(), pe.Y(), pe.Z() );
		Id( id );
	}

	static private int Execute(
							int id,
							double xs, double ys, double zs,
							double xe, double ye, double ze )
	{
		String cmd = "Create:Line:"
					 + " id=" + id
					 + ",sx=" + xs
					 + ",sy=" + ys
					 + ",sz=" + zs
					 + ",ex=" + xe
					 + ",ey=" + ye
					 + ",ez=" + ze;

		boolean okay = Portal.Execute( cmd );

		return (okay ? Portal.IntGet( "id" ) : 0);
	}
}

