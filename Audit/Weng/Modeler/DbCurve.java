
package Weng.Modeler;

import Weng.System.Portal;
import Weng.System.Const;
import Weng.Math.*;
import Weng.Geometry.Point;



/**
 * The class DbEntity is the abstract base class from which all other
 * entity classes are derived.  This class serves as a proxy to the
 * the actual database entity.  Being an abstract class, you can not
 * instantiate an instance of DbEntity.
 */

// NOTE: Really should be an abstract class
public class DbCurve extends DbEntity
{
	public DbCurve()
	{
		Id( 0 );
	}

	/**
	 * Copy constructor.
	 */
	public DbCurve( DbCurve dbCurve )
	{
		DbCurve	dbCopy;
		int		type;
		
		dbCopy = null;
		
		type = dbCurve.Type();
		
		if (type == Const.LINE)
		{
			dbCopy = new DbLine( DbLine.DbLine( dbCurve ) );
		}
		else if (type == Const.ARC)
		{
			dbCopy = new DbArc( DbArc.DbArc( dbCurve ) );
		}
		
		Id( ((dbCopy == null) ? 0 : dbCopy.Id()) );
	}
	
	/**
	 * Conversion operator.
	 */
	static public DbCurve DbCurve( DbEntity dbEntity )
	{
		int id = dbEntity.Id();
		if (id <= 0)
			return null;

		int type = dbEntity.Type();
		return ((type == Const.LINE || type == Const.ARC) ? new DbCurve( id ) : null);
	}

	/**
	 * Gets the start point of this curve entity as a geometric point.
	 */
	public Point StartPt()
	{
		String cmd = "Create:Extract: id=" + Id();
		boolean status = Portal.Execute( cmd );
		double xp = Portal.DoubleGet( "sx" );
		double yp = Portal.DoubleGet( "sy" );
		double zp = Portal.DoubleGet( "sz" );

		return (new Point( xp, yp, zp ));
	}

	/**
	 * Updates the start point position of this curve entity using the
	 * given geometric point.  With respect to arc entities, you must
	 * be very careful that the given point lies on the arc.
	 */
	public boolean StartPt( Point ps )
	{
		String cmd = null;
		int id = 0;

		int type = Type();
		Point pe = EndPt();

		switch( type )
		{
		case Const.LINE:
			id = DbLine.Execute( Id(), ps, pe );
			break;

		case Const.ARC:
			{
			DbArc dbArc = new DbArc( Id() );
			Point pc = dbArc.CenterPt();
			int dir = dbArc.Dir();
			id = DbArc.Execute( Id(), ps, pe, pc, dir );
			}
			break;
		}

		return (id > 0);
	}

	/**
	 * Gets the end point of this curve entity as a geometric point.
	 */
	public Point EndPt()
	{
		String cmd = "Create:Extract: id=" + Id();
		boolean status = Portal.Execute( cmd );
		double xp = Portal.DoubleGet( "ex" );
		double yp = Portal.DoubleGet( "ey" );
		double zp = Portal.DoubleGet( "ez" );

		return (new Point( xp, yp, zp ));
	}

	/**
	 * Updates the end point position of this curve entity using the
	 * given geometric point.  With respect to arc entities, you must
	 * be very careful that the given point lies on the arc.
	 */
	public boolean EndPt( Point pe )
	{
		String cmd = null;
		int id = 0;

		int type = Type();
		Point ps = StartPt();

		switch( type )
		{
		case Const.LINE:
			id = DbLine.Execute( Id(), ps, pe );
			break;

		case Const.ARC:
			{
			DbArc dbArc = new DbArc( Id() );
			Point pc = dbArc.CenterPt();
			int dir = dbArc.Dir();
			id = DbArc.Execute( Id(), ps, pe, pc, dir );
			}
			break;
		}

		return (id > 0);
	}

	/**
	 * Gets the mid point of this curve entity as a geometric point.
	 */
	public Point MidPt()
	{
		return ( Solver.CurvePt( this, Solver.MIDPOINT, 0.0 ) );
	}

	/**
	 * Gets a point on this curve entity as a geometric point.
	 * @param uparam A value in the range [0..100] percent
	 */
	public Point PtOnCurve( double uparam )
	{
		double dist = this.Length() * uparam;
		return ( Solver.CurvePt( this, Solver.FROMSTART, dist ) );
	}

	/**
	 * Gets the tangent at the start point of this curve entity.
	 */
	public UnitVec2d StartTan()
	{
		UnitVec2d vec = null;

		String cmd = "Solve:StartTan:id=" + Id();

		if ( Portal.Execute( cmd ) )
		{
			double dx = Portal.DoubleGet( "dx" );
			double dy = Portal.DoubleGet( "dy" );

			vec = new UnitVec2d( dx, dy );
		}

		return vec;
	}

	/**
	 * Gets the tangent at the end point of this curve entity.
	 */
	public UnitVec2d EndTan()
	{
		UnitVec2d vec = null;

		String cmd = "Solve:EndTan:id=" + Id();

		if ( Portal.Execute( cmd ) )
		{
			double dx = Portal.DoubleGet( "dx" );
			double dy = Portal.DoubleGet( "dy" );

			vec = new UnitVec2d( dx, dy );
		}

		return vec;
	}

	/**
	 * Gets the 3-dimensional arc length of this curve entity.
	 */
	public double Length()
	{
		double len = 0.0;

		String cmd = "Solve:Length:id=" + Id();

		if ( Portal.Execute( cmd ) )
			len = Portal.DoubleGet( "len" );
			 
		return len;
	}

	/**
	 * Reverses the direction of this curve entity.
	 */
	public boolean Reverse()
	{
		String cmd = "Profile:Reverse:id=" + Id();
		return ( Portal.Execute( cmd ) );
	}

	/**
	 * Creates a curve entity by offseting from this curve entity.
	 * @return (null) if this entity can not be offset using the given parameters
	 */
	public DbCurve Offset( double offsetDistance, int offsetDirection )
	{
		DbCurve dbCurve = null;

		String cmd = "Create:OffsetCurve:"
					 + " id=" + Id()
					 + ",dir=" + offsetDirection
					 + ",dist=" + offsetDistance;

		if ( Portal.Execute( cmd ) )
		{
			int id = Portal.IntGet( "id" );
			if (id > 0)
				dbCurve = new DbCurve( id );
		}

		return dbCurve;
	}

	/**
	 * Gets the start point of this curve as a point entity.
	 */
	public DbPoint DbStartPt()
	{
		DbPoint dbPoint = null;

		String cmd = "Solve:StartPt:id=" + Id();

		if ( Portal.Execute( cmd ) )
		{
			int id = Portal.IntGet( "id" );
			if (id > 0)
				dbPoint = new DbPoint( id );
		}

		return dbPoint;
	}

	/**
	 * Gets the end point of this curve as a point entity.
	 */
	public DbPoint DbEndPt()
	{
		DbPoint dbPoint = null;

		String cmd = "Solve:EndPt:id=" + Id();

		if ( Portal.Execute( cmd ) )
		{
			int id = Portal.IntGet( "id" );
			if (id > 0)
				dbPoint = new DbPoint( id );
		}

		return dbPoint;
	}


	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Protected methods.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	protected DbCurve( int id )
	{
		Id( id );
	}
}

