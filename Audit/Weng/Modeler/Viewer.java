
package Weng.Modeler;

import Weng.System.Portal;


/**
 * Use this class to update the view of the model as seen
 * through the user interface.
 */
public class Viewer
{
	/**
	 * Updates the view to show new entities and to remove
	 * display blemishes.
	 */
	static public boolean RefreshView()
	{
		boolean okay = Portal.Execute( "View:Refresh:" );

		return okay;
	}

	/**
	 * Scales the view such that all entities can be seen.
	 */
	static public boolean FullView()
	{
		boolean okay = Portal.Execute( "View:Full:regen=1" );

		return okay;
	}

	/**
	 * Turns text visibility on/off.
	 */
	static public boolean Text( boolean visible )
	{
		boolean okay = Portal.Execute( "View:Mode:text=" + (visible ? 1 : 0) );

		return okay;
	}

	/**
	 * Gets the text visibility status.
	 */
	static public boolean Text()
	{
		boolean visible = true;
		if ( Portal.Execute( "View:ModeGet:text=0" ) )
			visible = (Portal.IntGet( "text" ) != 0);
		return visible;
	}

	/**
	 * <b>Not yet implemented.</b>
	 */
	static public boolean NamedView( String viewName )
	{
		return false;
	}

	/**
	 * Sends the view graphics to the printer.
	 */
	static public boolean Print()
	{
		boolean verbatim = Verbatim();

		// By printing verbatim, we do move any text
		// that is outside the stock bounding box.

		Verbatim( true );
		boolean okay = Portal.Execute( "View:Print:" );
		Verbatim( verbatim );

		return okay;
	}

	static private boolean Verbatim( boolean active )
	{
		return ( Portal.Execute( "View:Mode:verbatim=" + (active ? 1 : 0) ) );
	}

	static private boolean Verbatim()
	{
		boolean verbatim = false;
		if ( Portal.Execute( "View:ModeGet:verbatim=0" ) )
			verbatim = (Portal.IntGet( "verbatim" ) != 0);
		return verbatim;
	}

	private Viewer()
	{
	}
}