
package Weng.Modeler;

import Weng.System.Portal;

/**
 * Use this singleton class to create and manipulate 'selection sets'
 * of entities, as is often operated upon by Weng.Modeler.Editor methods.
 * <p>
 * All Selector methods that operate on the modeler database do so
 * under the context of the current selection filter state.
 * <p>
 * Example:<br>
 *		Selector.All( true )     // enable selection of all entity types<br>
 *		Selector.Line( false )   // disable selection of lines<br>
 *		Selector.AddAll()        // selects everything except lines<br>
 */
public class Selector
{
	/**
	 * Gets the count of entities in this selection set.
	 */
	static public int Count()
	{
		int count = 0;

		String cmd = "Selector:Count:";
		boolean okay = Portal.Execute( cmd );

		if ( okay )
		{
			count = Portal.IntGet( "count" );
		}

		return count;
	}

	/**
	 * Gets the Ith entity in this selection set.
	 */
	static public DbEntity Get( int index )
	{
		DbEntity dbEntity = null;

		String cmd = "Selector:Get:index=" + index;
		boolean okay = Portal.Execute( cmd );

		if ( okay )
		{
			int id = Portal.IntGet( "id" );
			dbEntity = new DbEntity( id );
		}

		return dbEntity;
	}

	/**
	 * Adds an entity to the end of this selection set.
	 * @param entity (as a convenince measure is allowed to be <b>null</b>)
	 */
	static public boolean Add( DbEntity entity )
	{
		boolean okay = true;

		if (entity != null)
		{
			FiltersSet();
			okay = Portal.Execute( "Selector:Select:id=" + entity.Id() );
		}

		return okay;
	}

	/**
	 * Adds all entities that reference the given entity to
	 * the end of this selection set.  As an example, use
	 * this method when you want to select all entities that
	 * reference a given tool entity.
	 */
	static public boolean AddAllRefsTo( DbEntity entity )
	{
		boolean okay = true;

		if (entity != null)
		{
			String cmd = "Selector:SelectAllRefsTo:id=" + entity.Id();
			FiltersSet();
			okay = Portal.Execute( cmd );
		}

		return okay;
	}

	/**
	 * Adds all entities to this selection set.
	 */
	static public boolean AddAll()
	{
		FiltersSet();
		return Portal.Execute( "Selector:SelectAll:" );
	}

	/**
	 * Removes the given entity from this selection set.
	 */
	static public boolean Remove( DbEntity entity )
	{
		boolean okay = true;

		if (entity != null)
		{
			String cmd = "Selector:Unselect:id=" + entity.Id();
			FiltersSet();
			okay = Portal.Execute( cmd );
		}

		return okay;
	}

	/**
	 * Removes all entities from this selection set.
	 */
	static public boolean Flush()
	{
		String cmd = "Selector:Clear:";
		boolean okay = Portal.Execute( cmd );
		return okay;
	}

	/**
	 * Augments selection behavior.
	 * @param active (false) selects children whose parents pass the
	 *               selection filter / (true) selects only those
	 *               entities that pass the selection filter.
	 */
	static public boolean Restrictions( boolean active )
	{
		String cmd = "Selector:Restrictions: active=" + (( active ) ? 1 : 0);
		boolean okay = Portal.Execute( cmd );
		return okay;
	}

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Filter setting methods.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	/**
	 * Enables/disables the selection set filter such that all types
	 * of entities can/can not be added to the this selection set.
	 * @param enable (true) enable / (false) disable
	 */
	static public void All( boolean enable )
	{
		m_filters = ((enable) ? ALL : NONE);
	}

	/**
	 * Enables/disables the selection set filter such that tools
	 * can/can not be added to the this selection set.
	 * NOTE: Be very careful with the context in which you use
	 * this method.  For instance, do not delete any tools that
	 * are referenced!
	 * @param enable (true) enable / (false) disable
	 */
	static public void Tool( boolean enable )
	{
		if ( enable )
			m_filters |= TOOL;
		else
			m_filters &= ~TOOL;
	}

	/**
	 * Enables/disables the selection set filter such that workplanes
	 * can/can not be added to the this selection set.
	 * @param enable (true) enable / (false) disable
	 */
	static public void Workplane( boolean enable )
	{
		if ( enable )
			m_filters |= WORK;
		else
			m_filters &= ~WORK;
	}

	/**
	 * Enables/disables the selection set filter such that points
	 * can/can not be added to the this selection set.
	 * @param enable (true) enable / (false) disable
	 */
	static public void Point( boolean enable )
	{
		if ( enable )
			m_filters |= POINT;
		else
			m_filters &= ~POINT;
	}

	/**
	 * Enables/disables the selection set filter such that lines
	 * can/can not be added to the this selection set.
	 * @param enable (true) enable / (false) disable
	 */
	static public void Line( boolean enable )
	{
		if ( enable )
			m_filters |= LINE;
		else
			m_filters &= ~LINE;
	}

	/**
	 * Enables/disables the selection set filter such that arcs
	 * can/can not be added to the this selection set.
	 * @param enable (true) enable / (false) disable
	 */
	static public void Arc( boolean enable )
	{
		if ( enable )
			m_filters |= ARC;
		else
			m_filters &= ~ARC;
	}

	/**
	 * Enables/disables the selection set filter such that holes
	 * can/can not be added to the this selection set.
	 * @param enable (true) enable / (false) disable
	 */
	static public void Hole( boolean enable )
	{
		if ( enable )
			m_filters |= HOLE;
		else
			m_filters &= ~HOLE;
	}

	/**
	 * Enables/disables the selection set filter such that profiles
	 * can/can not be added to the this selection set.
	 *<p>
	 * Note: if you want to select a profile without selecting its
	 * curves, you will want to execute a sequence such as:
	 * <p>
	 *		Selector.All( false );     // disable all filters<br>
	 *		Selector.Profile( true );  // enable profiles only<br>
	 * @param enable (true) enable / (false) disable
	 */
	static public void Profile( boolean enable )
	{
		if ( enable )
			m_filters |= PROFILE;
		else
			m_filters &= ~PROFILE;
	}

	/**
	 * Enables/disables the selection set filter such that commands
	 * can/can not be added to the this selection set.
	 * @param enable (true) enable / (false) disable
	 */
	static public void Command( boolean enable )
	{
		if ( enable )
			m_filters |= COMMAND;
		else
			m_filters &= ~COMMAND;
	}

	/**
	 * Enables/disables the selection set filter such that features
	 * can/can not be added to the this selection set.
	 * @param enable (true) enable / (false) disable
	 */
	static public void Feature( boolean enable )
	{
		if ( enable )
			m_filters |= FEATURE;
		else
			m_filters &= ~FEATURE;
	}

	/**
	 * Enables/disables the selection set filter such that features
	 * can/can not be added to the this selection set.
	 * @param enable (true) enable / (false) disable
	 */
	static public void Sequence( boolean enable )
	{
		if ( enable )
			m_filters |= SEQUENCE;
		else
			m_filters &= ~SEQUENCE;
	}

	/**
	 * Enables/disables the selection set filter such that patterns
	 * can/can not be added to the this selection set.
	 * @param enable (true) enable / (false) disable
	 */
	static public void Pattern( boolean enable )
	{
		if ( enable )
			m_filters |= PATTERN;
		else
			m_filters &= ~PATTERN;
	}

	/**
	 * Saves the state of the selector.
	 */
	static public void StateSave()
	{
		Portal.Execute( "Selector:Push:" );
	}

	/**
	 * Restores the previous state of the selector.
	 */
	static public void StateRestore()
	{
		if ( Portal.Execute( "Selector:Pop:" ) )
			m_filters = Portal.IntGet( "filters" );
	}

	static private boolean FiltersSet()
	{
		String cmd = "Selector:Filter: encoded=" + m_filters;

		boolean okay = Portal.Execute( cmd );

		return okay;
	}

	private Selector()
	{
	}

	static private final int NONE		= 0x0;
	// static private final int LAYER		= 0x001;  OBSOLETE
	static private final int WORK		= 0x002;
	static private final int TOOL		= 0x004;
	static private final int POINT		= 0x008;
	static private final int LINE		= 0x010;
	static private final int ARC		= 0x020;
	static private final int HOLE		= 0x040;
	static private final int PROFILE	= 0x080;
	static private final int COMMAND	= 0x100;
	static private final int FEATURE	= 0x200;
	static private final int SEQUENCE	= 0x400;
	static private final int PATTERN	= 0x800;
	static private final int ALL		= 0xFFF;

	static int m_filters = ALL;
}