
package Weng.Modeler;

import Weng.System.Portal;
import Weng.System.Const;
import Weng.Geometry.Point;


/**
 * Use this class to create, query and manipulate a hole
 * entity that resides in the modeler's database.
 */
public class DbCommand extends DbEntity
{
	/**
	 * Creates a command entity at the given location and having the given text.
	 */
	public DbCommand( Point pt, String text )
	{
		String cmd = "Create:Command:"
					 + " x=" + pt.X()
					 + ",y=" + pt.Y()
					 + ",z=" + pt.Z()
					 + ",cmd=\"" + text + "\"";

		if ( Portal.Execute( cmd ) )
		{
			int id = Portal.IntGet( "id" );
			Id( id );
		}
	}

	/**
	 * Creates a command entity at the given location and having the given text.
	 */
	public DbCommand( double x, double y, double z, String text )
	{
		String cmd = "Create:Command:"
					 + " x=" + x
					 + ",y=" + y
					 + ",z=" + z
					 + ",cmd=\"" + text + "\"";

		if ( Portal.Execute( cmd ) )
		{
			int id = Portal.IntGet( "id" );
			Id( id );
		}
	}

	/**
	 * Copy constructor.  Creates a unique copy of the given arc entity.
	 * @param dbArc the existing arc entity to be copied
	 */
	public DbCommand( DbCommand dbCommand )
	{
		Point pt = dbCommand.Point();
		
		String cmd = "Create:Command:"
					 + " x=" + pt.X()
					 + ",y=" + pt.Y()
					 + ",z=" + pt.Z()
					 + ",cmd=\"" + dbCommand.Text() + "\"";

		if ( Portal.Execute( cmd ) )
		{
			int id = Portal.IntGet( "id" );
			Id( id );
		}
	}

	/**
	 * Conversion operator.
	 */
	static public DbCommand DbCommand( DbEntity dbEntity )
	{
		int id = dbEntity.Id();
		if (id <= 0)
			return null;

		if (dbEntity.Type() != Const.COMMAND)
			return null;

		return (new DbCommand( id ));
	}

	/**
	 * Gets this command entity's location.
	 */
	public Point Point()
	{
		Point pt = null;

		if ( Extract() )
		{
			double xp = Portal.DoubleGet( "x" );
			double yp = Portal.DoubleGet( "y" );
			double zp = Portal.DoubleGet( "z" );
			pt = new Point( xp, yp, zp );
		}

		return pt;
	}

	/**
	 * Gets this command entity's text.
	 */
	public String Text()
	{
		return (Extract() ? Portal.StringGet( "text" ) : null);
	}


	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Protected methods.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	protected DbCommand( int id )
	{
		Id( id );
	}

	/**
	 * Gets this holes geometric surface center point.
	 */
	private boolean Extract()
	{
		String cmd = "Create:Extract: id=" + Id();
		return ( Portal.Execute( cmd ) );
	}
}

