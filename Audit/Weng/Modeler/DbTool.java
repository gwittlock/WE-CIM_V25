
package Weng.Modeler;

import Weng.System.Portal;
import Weng.System.Const;

/**
 * Use this class to create a proxy to a tool entity in
 * the modeler's database.  Tool entities can neither be
 * created nor manipulated.
 *<p>
 * When writing macros, obtain a tool proxy by using a
 * call to Weng.Modeler.Model.ToolGet( int toolNumber )
 */
public class DbTool extends DbEntity
{
	// Tools can not be instantiated currently.
	//    See also Weng.Modeler.Model.ToolGet()
	
	/**
	 * Creates a tool (layer) entity having the given name
	 * and that is associated with the active workplane.
	 */
	public DbTool( String name )
	{
		String cmd = "Create:Tool: name=\"" + name + "\"";

		if ( Portal.Execute( cmd ) )
		{
			int id = Portal.IntGet( "id" );
			Id( id );
		}
	}

	/**
	 * Conversion operator.
	 */
	static public DbTool DbTool( DbEntity dbEntity )
	{
		int id = dbEntity.Id();
		if (id <= 0)
			return null;

		if (dbEntity.Type() != Const.TOOL)
			return null;
			
		return (new DbTool( id ));
	}

	public boolean IsLayer()
	{
		int	type = IntGet("Type_ID");
		return (type >= (int) Const.UNDEFINED);
	}
	
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Protected methods.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	protected DbTool( int id )
	{
		Id( id );
	}
}

