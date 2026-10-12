
package Weng.Modeler;

import Weng.System.Portal;
import Weng.System.Const;


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// This class does not contain JavaDoc style comments
// because its use is not intended to be exposed to
// clients of the package Weng.Modeler
//
// NOTE: Really should be an abstract class
// public abstract class DbContainer extends DbEntity
public class DbContainer extends DbEntity
{
	/**
	 * Conversion operator.
	 */
	static public DbContainer DbContainer( DbEntity dbEntity )
	{
		int id = dbEntity.Id();
		if (id <= 0)
			return null;

		int type = dbEntity.Type();
		return ((type == Const.PROFILE || type == Const.FEATURE) ? new DbContainer( id ) : null);
	}
	
	/**
	 * Obtain the count of entities owner by this container.
	 */
	public int Count()
	{
		int count = 0;

		String cmd = "Entity:Container:Count:id=" + Id();

		if ( Portal.Execute( cmd ) )
			count = Portal.IntGet( "count" );

		return count;
	}

	/**
	 * Obtain a pointer to the Ith entity.
	 */
	public DbEntity Get( int indx )
	{
		DbEntity result = null;

		String cmd = "Entity:Container:Get:id=" + Id() + ",index=" + indx;

		if ( Portal.Execute( cmd ) )
		{
			int id = Portal.IntGet( "id" );
			if (id > 0)
				result = new DbEntity( id );
		}

		return result;
	}

	/**
	 * Replace an entity at a known position.
	 * @return the previous entity at the known position.
	 */
	public DbEntity Replace( int indx, DbEntity dbEntity )
	{
		DbEntity result = null;

		String cmd = "Entity:Container:Set:" +
			" id=" + Id() + ",index=" + indx + ",entid=" + dbEntity.Id();

		if ( Portal.Execute( cmd ) )
		{
			int id = Portal.IntGet( "id" );
			if (id > 0)
				result = new DbEntity( id );  // the id of the prev entity
		}

		return result;
	}
	
	/**
	 * Add an entity to this container.
	 */
	public int Append( DbEntity dbEntity )
	{
		int count = 0;

		if (Id() != dbEntity.Id())
		{
			String cmd = "Entity:Container:Append:"
						 + " id=" + Id()
						 + ",newid=" + dbEntity.Id()
						 + ",copy=0";
	
			if ( Portal.Execute( cmd ) )
				count = Portal.IntGet( "count" );
		}
	
		return count;
	}

	/**
	 * Insert an entity in front of a known entity in this container.
	 */
	public int InsertBefore( DbEntity refEntity, DbEntity newEntity )
	{
		int indx = -1;

		if (Id() != newEntity.Id())
		{
			String cmd = "Entity:Container:InsertBefore:"
						 + " id=" + Id()
						 + ",refid=" + refEntity.Id()
						 + ",newid=" + newEntity.Id();
	
			if ( Portal.Execute( cmd ) )
				indx = Portal.IntGet( "index" );
		}
	
		return indx;
	}

	/**
	 * Insert an entity behind a known entity in this container.
	 */
	public int InsertAfter( DbEntity refEntity, DbEntity newEntity )
	{
		int indx = -1;

		if (Id() != newEntity.Id())
		{
			String cmd = "Entity:Container:InsertAfter:"
						 + " id=" + Id()
						 + ",refid=" + refEntity.Id()
						 + ",newid=" + newEntity.Id();
	
			if ( Portal.Execute( cmd ) )
				indx = Portal.IntGet( "index" );
		}
	
		return indx;
	}

	/**
	 * Find the index of this entity in this container.
	 * @return (-1) not found
	 */
	public int IndexOf( DbEntity dbEntity )
	{
		DbEntity	candidate;
		int			count, indx;
		
		count = this.Count();
		for (indx = 0; indx < count; ++indx)
		{
			candidate = this.Get(indx);
			if (candidate.Id() == dbEntity.Id())
				break;
		}
		
		return ((indx < count) ? indx : -1);
	}
	
	/**
	 * Release ownership of the given entity.
	 * Returns true if successful.
	 */
	public boolean Disown( DbEntity dbEntity )
	{
		String cmd = "Entity:Container:Disown:"
					 + " id=" + Id()
					 + ",refid=" + dbEntity.Id();

		return ( Portal.Execute( cmd ) );
	}

	/**
	 * Non-destructively remove the entities from this container.
	 */
	public boolean BenignFlush()
	{
		String cmd = "Entity:Container:BenignFlush:id=" + Id();
		return ( Portal.Execute( cmd ) );
	}

	/**
	 * Destructively remove the entities from this container.
	 */
	public boolean DestructiveFlush()
	{
		String cmd = "Entity:Container:DestructiveFlush:id=" + Id();
		return ( Portal.Execute( cmd ) );
	}


	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Protected methods.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	protected DbContainer( int id )
	{
		Id( id );
	}

	protected DbContainer()
	{
	}
}

