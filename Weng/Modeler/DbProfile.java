
package Weng.Modeler;

import Weng.System.Portal;
import Weng.System.Const;


/**
 * Use this class to create, query and manipulate a profile
 * entity that resides in the modeler's database.  A profile
 * is an ordered sequence of curves.
 */
public class DbProfile extends DbContainer
{
	/**
	 * Creates an empty profile entity.
	 */
	public DbProfile()
	{
		String cmd = "Profile:Create:";

		if ( Portal.Execute( cmd ) )
		{
			int id = Portal.IntGet( "id" );
			Id( id );
		}
	}

	/**
	 * Conversion operator.
	 */
	static public DbProfile DbProfile( DbEntity dbEntity )
	{
		int id = dbEntity.Id();
		if (id <= 0)
			return null;

		if (dbEntity.Type() != Const.PROFILE)
			return null;
			
		return (new DbProfile( id ));
	}

	/**
	 * Determines whether this is a 'closed' profile based on whether
	 * the starting and end points of this profile are coincident.
	 */
	public boolean IsClosed()
	{
		boolean closed = false;

		String cmd = "Profile:IsClosed: id=" + Id();

		if ( Portal.Execute( cmd ) )
			closed = (Portal.IntGet( "bool" ) != 0);

		return closed;
	}

	/**
	 * Gets the signed area of this profile (+) CCW / (-) CW.
	 * @return (Const.UNDEFINED) failure
	 */
	public double Area()
	{
		String cmd = "Solve:Area: id=" + Id();

		return (Portal.Execute( cmd ) ? Portal.DoubleGet( "area" ) : Const.UNDEFINED);
	}

	/**
	 * Adds a curve entity to the end of this profile.  Note that
	 * a curve entity can belong to only one profile.
	 */
	public boolean Append( DbCurve dbCurve )
	{
		String cmd = "Profile:Modify:"
					 + " id=" + Id()
					 + ",add=" + dbCurve.Id();

		return ( Portal.Execute( cmd ) );
	}

	/**
	 * Forces adjacent curves in this profile to share common end points.
	 * Assuming we have an unassociated profile containing two curves, the
	 * profile will have four points.  If the two of the curve end points
	 * overlap within the given gap tolerance, the equivalent associated
	 * profile will have only three points, introducing 'rubber band'
	 * behavior.  If you modify the common end point, both curves will
	 * likewise be modified.
	 */
	public boolean Associate( double gapTol )
	{
		String cmd = "Profile:Associate:id=" + Id() + ",tol=" + gapTol;
		return ( Portal.Execute( cmd ) );
	}

	/**
	 * Forces each curve in this profile to be unique and separate from
	 * its adjacent curves.  Whereas the adjacent curves of an associated
	 * profile may reference common end points, the curves in an unassociated
	 * profile reference separate end points.
	 */
	public boolean Disassociate()
	{
		String cmd = "Profile:Disassociate:id=" + Id();
		return ( Portal.Execute( cmd ) );
	}

	/**
	 * Reverse the order and direction of curves in this profile.
	 */
	public boolean Reverse()
	{
		String cmd = "Profile:Reverse:id=" + Id();
		return ( Portal.Execute( cmd ) );
	}

	/**
	 * Creates a feature containing one or more profiles, each profile containing
	 * offset curves derives from the curves of this profile.
	 * @return The result will normally contain only one profile, but certain
	 * geometric conditions can lead to a 'bifurcation' in the result.  Regardless,
	 * each resulting profile will be devoid of 'loops'.
	 */
	public DbFeature Offset(
						int offsetDirection,
						double offsetDistance,
						double sharpAngle,
						double zLevel )
	{
		DbFeature dbFeature = null;
		if (Count() > 0)
		{
			DbTool dbTool = this.Get(0).Tool();
			int toolId = ((dbTool == null) ? 0 : dbTool.Id());
			dbFeature = Offset( Id(), toolId,
				offsetDirection, offsetDistance, sharpAngle, zLevel );

		}
		
		return dbFeature;
	}

	/**
	 * Gets this signed direction of this profile.
	 * @return Signed 'sense of direction' (+) CCW / (-) CW<br>
	 * Note that 'open' profiles can yield ambiguous results.
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


	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Protected methods.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	static protected DbFeature Offset(
						int profId,
						int toolId,
						int offsetDirection,
						double offsetDistance,
						double sharpAngle,
						double zLevel )
	{
		DbFeature dbFeature = null;

		DbTool activeTool = Model.ActiveToolGet();

		// Note: Suppress explosion.  Otherwise, the feature will be empty.
		String cmd = "Create:OffsetProfile:"
					 + " profid=" + profId
					 + ",toolid=" + toolId
					 + ",dir=" + offsetDirection
					 + ",dist=" + offsetDistance
					 + ",sharp=" + sharpAngle
					 + ",zlevel=" + zLevel
					 + ",explode=0";

		if ( Portal.Execute( cmd ) )
		{
			int id = Portal.IntGet( "id" );
			if (id > 0)
				dbFeature = new DbFeature( id );
		}

		if (toolId > 0 && dbFeature != null)
			Model.ActiveToolSet( activeTool );

		return dbFeature;
	}

	static protected DbFeature Offset(
						int profId,
						int toolId,
						boolean climbCut,
						double offsetDistance,
						double sharpAngle,
						double zLevel )
	{
		DbFeature dbFeature = null;

		DbTool activeTool = Model.ActiveToolGet();

		// Note: Suppress explosion.  Otherwise, the feature will be empty.
		String cmd = "Create:OffsetProfile:"
					 + " profid=" + profId
					 + ",toolid=" + toolId
					 + ",climb=" + (( climbCut ) ? 1 : 0)
					 + ",dist=" + offsetDistance
					 + ",sharp=" + sharpAngle
					 + ",zlevel=" + zLevel
					 + ",explode=0";

		if ( Portal.Execute( cmd ) )
		{
			int id = Portal.IntGet( "id" );
			if (id > 0)
				dbFeature = new DbFeature( id );
		}

		if (toolId > 0 && dbFeature != null)
			Model.ActiveToolSet( activeTool );

		return dbFeature;
	}

	protected DbProfile( int id )
	{
		Id( id );
	}
}

