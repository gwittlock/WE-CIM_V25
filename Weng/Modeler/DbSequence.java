
package Weng.Modeler;

import Weng.System.Portal;
import Weng.System.Const;
import Weng.Geometry.Point;


/**
 * Use this class to create and manipulate a sequence
 * entity that resides in the modeler's database.
 * <p>
 * Do not attempt to use this class unless you have
 * a firm understanding of this products architecture.
 */
public class DbSequence extends DbEntity
{
	/**
	 * Creates a point entity using the given ordinate data.
	 */
	public DbSequence( String name )
	{
		String cmd = "Seq:Create:Name=\"" + name + "\"";
		
		if ( Portal.Execute( cmd ) )
		{
			int id = Portal.IntGet( "id" );
			Id( id );
		}
	}

	/**
	 * Obtains the count of entities contained by this sequence.
	 * @return the count of entities
	 */
	public int Count()
	{
		String	cmd = "Seq:Count:id=" + Id();
		return (Portal.Execute( cmd ) ? Portal.IntGet("count") : 0);
	}
	
	/**
	 * Obtains the Ith entity contained by this sequence.
	 * @return (null) failed / (?) the entity
	 */
	public DbEntity Get( int indx )
	{
		DbEntity	dbEntity = null;
		String		cmd = "Seq:Get:id=" + Id() + ",indx=" + indx;
		
		if ( Portal.Execute( cmd ) )
		{
			int id = Portal.IntGet("id");
			dbEntity = new DbEntity();
			dbEntity.Id( id );
		}
		
		return dbEntity;
	}
	
	/**
	 * Appends the given entity to the end of this sequence.
	 */
	public boolean Append( DbEntity dbEntity )
	{
		String	cmd = "Seq:Append:"
				+ " parent=" + Id()
				+ ",child=" + dbEntity.Id();
				
		return ( Portal.Execute( cmd ) );
	}

	/**
	 * Non-destructively remove the entities from this sequence.
	 */
	public boolean BenignFlush()
	{
		String cmd = "Seq:BenignFlush:id=" + Id();
		return ( Portal.Execute( cmd ) );
	}
	
	/**
	 * Conversion operator.
	 */
	static public DbSequence DbSequence( DbEntity dbEntity )
	{
		int id = dbEntity.Id();
		if (id <= 0)
			return null;

		if (dbEntity.Type() != Const.SEQUENCE)
			return null;
			
		return (new DbSequence( id ));
	}


	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Protected methods.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	protected DbSequence( int id )
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

