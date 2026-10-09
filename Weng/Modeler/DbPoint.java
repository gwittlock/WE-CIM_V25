
package Weng.Modeler;

import Weng.System.Portal;
import Weng.System.Const;
import Weng.Geometry.Point;


/**
 * Use this class to create, query and manipulate a point
 * entity that resides in the modeler's database.
 * <p>
 * If you want to get ordinate data from a point entity,
 * make call such as myPoint.Point().X()
 */
public class DbPoint extends DbEntity
{
	/**
	 * Creates a point entity using the given ordinate data.
	 */
	public DbPoint( double x, double y, double z )
	{
		Execute( 0, x, y, z );
	}

	/**
	 * Creates a point entity using the properties of the
	 * given geometric point.
	 */
	public DbPoint( Point pt )
	{
		Execute( 0, pt.X(), pt.Y(), pt.Z() );
	}

	/**
	 * Copy constructor.  Creates a unique copy of the given point entity.
	 * @param dbPoint the existing point entity to be copied
	 */
	public DbPoint( DbPoint dbPoint )
	{
		Point pt = dbPoint.Point();
		Execute( 0, pt.X(), pt.Y(), pt.Z() );
	}

	public void Assign( double x, double y, double z )
	{
		Execute( Id(), x, y, x );
	}

	/**
	 * Gets the geometric equivalent of this point entity.
	 */
	public Point Point()
	{
		String cmd = "Create:Extract: id=" + Id();
		boolean status = Portal.Execute( cmd );
		double xp = Portal.DoubleGet( "x" );
		double yp = Portal.DoubleGet( "y" );
		double zp = Portal.DoubleGet( "z" );

		return (new Point( xp, yp, zp ));
	}

	/**
	 * Conversion operator.
	 */
	static public DbPoint DbPoint( DbEntity dbEntity )
	{
		int id = dbEntity.Id();
		if (id <= 0)
			return null;

		if (dbEntity.Type() != Const.POINT)
			return null;
			
		return (new DbPoint( id ));
	}


	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Protected methods.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	protected DbPoint( int id )
	{
		Id( id );
	}

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Private methods.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	private void Execute( int id, double x, double y, double z )
	{
		String cmd = "Create:Point:"
					 + " id=" + id		 
					 + ",x=" + x
					 + ",y=" + y
					 + ",z=" + z;

		if ( Portal.Execute( cmd ) )
		{
			id = Portal.IntGet( "id" );
			Id( id );
		}
	}
}

