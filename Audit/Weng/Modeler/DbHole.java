
package Weng.Modeler;

import Weng.System.Portal;
import Weng.System.Const;
import Weng.Geometry.Point;


/**
 * Use this class to create, query and manipulate a hole
 * entity that resides in the modeler's database.
 */
public class DbHole extends DbEntity
{
	/**
	 * Creates a hole entity whose top center point is at the
	 * given location and having the given diameter and depth.
	 */
	public DbHole( double x, double y, double z, double diam, double depth )
	{
		String cmd = "Create:Hole:"
					 + " xc=" + x
					 + ",yc=" + y
					 + ",zc=" + z
					 + ",dia=" + diam
					 + ",depth=" + depth;

		if ( Portal.Execute( cmd ) )
		{
			int id = Portal.IntGet( "id" );
			Id( id );
		}
	}

	/**
	 * Conversion operator.
	 */
	static public DbHole DbHole( DbEntity dbEntity )
	{
		int id = dbEntity.Id();
		if (id <= 0)
			return null;

		if (dbEntity.Type() != Const.HOLE)
			return null;
						
		return (new DbHole( id ));
	}

	/**
	 * Gets this holes geometric surface center point.
	 */
	public Point CenterPt()
	{
		String cmd = "Create:Extract: id=" + Id();
		boolean status = Portal.Execute( cmd );
		double xp = Portal.DoubleGet( "x" );
		double yp = Portal.DoubleGet( "y" );
		double zp = Portal.DoubleGet( "z" );

		return (new Point( xp, yp, zp ));
	}


	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Protected methods.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	protected DbHole( int id )
	{
		Id( id );
	}
}

