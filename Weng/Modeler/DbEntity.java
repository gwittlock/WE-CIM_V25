
package Weng.Modeler;

import Weng.System.Portal;
import Weng.System.Const;
import Weng.Math.*;
import java.awt.Color;

/**
 * The class DbEntity is the abstract base class from which all other
 * entity classes are derived.  This class serves as a proxy to
 * the actual database entity.  Being an abstract class, you can
 * not instantiate an instance of DbEntity.
 */
public class DbEntity
{
	/**
	 * Gets this entity's type.
	 * @return returns one of the enumerated types declared in Weng.System.Const
	 * <p>
	 * Const.WORKPLANE<br>
	 * Const.TOOL<br>
	 * Const.POINT<br>
	 * Const.LINE<br>
	 * Const.ARC<br>
	 * Const.HOLE<br>
	 * Const.PROFILE<br>
	 * Const.COMMAND<br>
	 * Const.FEATURE<br>
	 * Const.INSTANCE<br>
	 * Const.MACRO<br>
	 * Const.GROUP
	 */
	public int Type()
	{
		int type = -1;

		String cmd = "Entity:Type: id=" + m_id;
		boolean okay = Portal.Execute( cmd );
		if ( okay )
			type = Portal.IntGet( "type" );

		return type;
	}

	/**
	 * Gets the name of this entity.
	 * @return (_name) system default names always start with the underscore
	 *         character.  Otherwise, the returned name is a user-defined name.
	 */
	public String Name()
	{
		String name = null;

		String cmd = "Entity:Name: id=" + m_id;

		if ( Portal.Execute( cmd ) )
			name = Portal.StringGet( "name" );

		return name;
	}

	/**
	 * Sets the name of this entity.
	 * @param name this entity's new name.  The naming convention follows:<br>
	 * 1) case is ignored,<br>
	 * 2) the first character must be in [a-z]<br>
	 * 3) subsequent characters must be in [a-z,0-9,_]
	 * @return (true) successful / (false) failed for because 1) the name does
	 * not adhere to naming conventions or 2) the name is already being used by
	 * another entity.
	 */
	public boolean Name( String name )
	{
		boolean okay = false;
		
		String cmd = "Entity:Name: id=" + m_id + ",name=\"" + name + "\"";

		if ( Portal.Execute( cmd ) )
			okay = (Portal.IntGet( "bool" ) > 0);

		return okay;
	}

	/**
	 * Gets the tool (layer) entity that this entity resides on.
	 * @return (null) when this entity does not reside on a tool, as is
	 *         the case when this entity is a tool, workplane or tool.
	 */
	public DbTool Tool()
	{
		DbTool dbTool = null;

		String cmd = "*Entity:Tool: id=" + m_id;

		if ( Portal.Execute( cmd ) )
		{
			int toolId = Portal.IntGet( "id" );
			if (toolId > 0)
				dbTool = new DbTool( toolId );
		}

		return dbTool;
	}

	/**
	 * Sets the tool (layer) entity that this entity resides on.
	 * This has the side effect of updating the tool reference.
	 * @return (true) success
	 */
	public boolean Tool( DbTool tool )
	{
		int id = 0;

		String cmd = "Entity:Tool:" +
					 " id=" + m_id +
					 ",tool=" + tool.Id();

		if ( Portal.Execute( cmd ) )
			id = Portal.IntGet( "id" );

		return (id != 0);
	}

	/**
	 * Gets the workplane entity that this entity references.
	 * @return (null) when this entity does not reference a workplane,
	 *         as is the case when this entity is a container.
	 */
	public DbWorkplane Workplane()
	{
		DbWorkplane dbWork = null;

		String cmd = "Entity:Work: id=" + m_id;

		if ( Portal.Execute( cmd ) )
		{
			int workId = Portal.IntGet( "id" );
			if (workId > 0)
				dbWork = new DbWorkplane( workId );
		}

		return dbWork;
	}

	/**
	 * Gets this entity's color.
	 */
	public int Color()
	{
		int color = IntGet( "color" );
		return ((color == (int) Const.UNDEFINED) ? 255 : color);
	}

	/**
	 * Sets this entity's color.
	 */
	public void Color( int color )
	{
		IntSet( "color", color );
	}

	/**
	 * Gets the reference count to this entity.
	 */
	public int RefCount()
	{
		int count = 0;

		String cmd = "Entity:RefCnt: id=" + m_id;

		if ( Portal.Execute( cmd ) )
			count = Portal.IntGet( "count" );

		return count;
	}

	public DbEntity Owner()
	{
		DbEntity dbOwner = null;

		String cmd = "Create:Extract: id=" + m_id;

		if ( Portal.Execute( cmd ) )
		{
			int owner = Portal.IntGet( "owner" );
			if (owner > 0)
				dbOwner = new DbEntity( owner );
		}

		return dbOwner;
	}

	/**
	 * Hides this entity (so that it can not be selected, for instance).
	 */
	public boolean Hide()
	{
		String cmd = "Entity:Hide: id=" + Id();
		return (Portal.Execute( cmd ));
	}

	/**
	 * Reverts this entity's hidden status.
	 */
	public boolean Show()
	{
		String cmd = "Entity:Show: id=" + Id();
		return (Portal.Execute( cmd ));
	}

	/**
	 * Causes this entity to become a 'system entity'.
	 */
	public boolean System()
	{
		String cmd = "Entity:System: id=" + Id();
		return (Portal.Execute( cmd ));
	}

	/**
	 * Causes this entity to become a 'user entity'.
	 */
	public boolean User()
	{
		String cmd = "Entity:User: id=" + Id();
		return (Portal.Execute( cmd ));
	}

	/**
	 * Gets the 3-dimensional bounding box of this entity.
	 * @return (null) indicates failure
	 */
	public Box3d Box()
	{
		Box3d box = null;

		String cmd = "Entity:Box:id=" + m_id;

		boolean okay = Portal.Execute( cmd );

		if ( okay )
		{
			double xmin = Portal.DoubleGet( "xmin" );
			double ymin = Portal.DoubleGet( "ymin" );
			double zmin = Portal.DoubleGet( "zmin" );
			double xmax = Portal.DoubleGet( "xmax" );
			double ymax = Portal.DoubleGet( "ymax" );
			double zmax = Portal.DoubleGet( "zmax" );

			box = new Box3d( xmin, ymin, zmin, xmax, ymax, zmax );
		}

		return box;
	}

	/**
	 * Gets the value of a named-attribute that is attached to this
	 * entity, as an integer value.
	 * @return (Const.UNDEFINED) if the named-attribute does not exist
	 */
	public int IntGet( String attribName )
	{
		int result = (int) Const.UNDEFINED;

		String cmd = "*Attrib:Get:id=" + Id() + ",name=\"" + attribName + "\"";
		if ( Portal.Execute( cmd ) )
		{
			result = Portal.IntGet( "val" );
		}

		return result;
	}

	/**
	 * Gets the value of a named-attribute that is attached to this
	 * entity, as a double value.
	 * @return (Const.UNDEFINED) if the named-attribute does not exist
	 */
	public double DoubleGet( String attribName )
	{
		double result = Const.UNDEFINED;

		String cmd = "*Attrib:Get:id=" + Id() + ",name=\"" + attribName + "\"";
		if ( Portal.Execute( cmd ) )
		{
			result = Portal.DoubleGet( "val" );
		}

		return result;
	}

	/**
	 * Gets the value of a named-attribute that is attached to this
	 * entity, as a string value.
	 * @return (null) if the named-attribute does not exist
	 */
	public String StringGet( String attribName )
	{
		String result = null;

		String cmd = "*Attrib:Get:id=" + Id() + ",name=\"$" + attribName + "\"";
		if ( Portal.Execute( cmd ) )
		{
			result = Portal.StringGet( "val" );
		}

		return result;
	}

	/**
	 * Attaches an integer value named-attribute to this entity.
	 */
	public boolean IntSet( String attribName, int value )
	{
		String cmd = "Attrib:Set:"
					 + " id=" + Id()
					 + ",name=\"#" + attribName + "\""
					 + ",val=" + value;

		return ( Portal.Execute( cmd ) );
	}

	/**
	 * Attaches a double value named-attribute to this entity.
	 */
	public boolean DoubleSet( String attribName, double value )
	{
		String cmd = "Attrib:Set:"
					 + " id=" + Id()
					 + ",name=\"" + attribName + "\""
					 + ",val=" + value;

		return ( Portal.Execute( cmd ) );
	}

	/**
	 * Attaches a string value named-attribute to this entity.
	 */
	public boolean StringSet( String attribName, String value )
	{
		String cmd = "Attrib:Set:"
					 + " id=" + Id()
					 + ",name=\"$" + attribName + "\""
					 + ",val=\"" + value + "\"";

		return ( Portal.Execute( cmd ) );
	}

	/**
	 * Removes the named-attribute from this entity.
	 */
	public boolean AttribDelete( String attribName )
	{
		String cmd = "Attrib:Del:"
					 + " id=" + Id()
					 + ",name=\"" + attribName + "\"";

		return ( Portal.Execute( cmd ) );
	}

	/**
	 * Removes this entity from the modeler database.
	 */
	public void Delete()
	{
		// NOTE: If this entity has an id of zero
		// (eg. this entity has already been deleted)
		// then the system will issue an error message.

		String cmd = "Create:Delete:id=" + m_id;
		Portal.Execute( cmd );
		m_id = 0;
	}

	/**
	 * Creates a unique copy of this entity in the modeler database.
	 */
	public DbEntity Clone()
	{
		int cloneId = Clone( m_id );
		
		return ((cloneId > 0) ? new DbEntity( cloneId ) : null);
	}

	public boolean IsLeadIn()
	{
		boolean isLead = false;

		String cmd = "*Entity:IsLead: id=" + m_id + ",type=0";

		if ( Portal.Execute( cmd ) )
			isLead = (Portal.IntGet( "bool" ) != 0);

		return isLead;
	}

	public boolean IsLeadOut()
	{
		boolean isLead = false;

		String cmd = "*Entity:IsLead: id=" + m_id + ",type=1";

		if ( Portal.Execute( cmd ) )
			isLead = (Portal.IntGet( "bool" ) != 0);

		return isLead;
	}

	/**
	 * Appends all references to this entity to the given array.
	 */
	public DbEntity [] RefsTo()
	{
		DbEntity [] array;
		boolean		okay;
		int			count, indx;
		int			id;
		
		array = null;
		
		okay = Portal.Execute( "Entity:RefsToInit:id=" + m_id );
		if ( okay )
		{
			okay = Portal.Execute( "Entity:RefsToCount:" );
			if ( okay )
			{
				count = Portal.IntGet( "count" );
				if (count > 0)
				{
					array = new DbEntity[count];
					
					for (indx = 0; indx < count; ++indx)
					{
						Portal.Execute( "Entity:RefsToGet:indx=" + indx );
						id = Portal.IntGet( "id" );
						
						array[indx] = new DbEntity( id );
					}
				}
			}
		}
		
		return array;
	}
	
	/**
	 * <b>USE WITH CAUTION.</b> Creates a proxy having an invalid id.
	 */
	public DbEntity()
	{
		m_id = 0;
	}

	/**
	 * <b>USE WITH CAUTION.</b> Updates this proxy.
	 */
	public void Id( int id )
	{
		m_id = id;
	}

	/**
	 * Calculates a red-green-blue color value using the red, green
	 * and blue proportions in the range 0..255 inclusive.
	 */
	static public int RGB( int red, int green, int blue )
	{
		String cmd = "View:RGB:" +
					 " red=" + red +
					 ",green=" + green +
					 ",blue=" + blue;

		Portal.Execute( cmd );

		return ( Portal.IntGet( "rgb" ) );
	}

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Protected methods
	// NOTE: The constructors are protected because DbEntity
	// is essentially an abstract base class.  As such we
	// want to prevent object instantiation with these
	// constructors outside of package Modeler.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	/**
	 * <b>USE WITH CAUTION.</b>  Gets the persistent database id of this entity.
	 */
	public int Id()
	{
		return m_id;
	}

	protected DbEntity( int id )
	{
		m_id = id;
	}


	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Private methods
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	static private native int  Clone( int entityId );


	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Private data
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	// The database id of this entity.  Java's 'int' is the 32-bit
	// equivalent of MFVC++'s DWORD.  A DWORD is unsigned, however.
	private int m_id;


	static
	{
		String libName = "mm2portal";

		try
		{
			System.loadLibrary( libName );
		}
		catch (UnsatisfiedLinkError e)
		{
			System.out.println( "Fatal Error: class DbEntity failed to load library "+libName );
		}
	}
}

