
package Weng.Modeler;

import Weng.System.Portal;
import Weng.System.Const;

/**
 * Use this class to create, query and manipulate a feature
 * entity that resides in the modeler's database.
 */
public class DbFeature extends DbContainer
{
	/**
	 * Creates an empty feature entity that has no reference to a tool entity.
	 */
	public DbFeature()
	{
		if ( Portal.Execute("Feature:Create:") )
		{
			int id = Portal.IntGet( "id" );
			Id( id );
		}
	}

	/**
	 * Conversion operator.
	 */
	static public DbFeature DbFeature( DbEntity dbEntity )
	{
		int id = dbEntity.Id();
		if (id <= 0)
			return null;

		if (dbEntity.Type() != Const.FEATURE)
			return null;
			
		return (new DbFeature( id ));
	}

	/**
	 * Divorces this feature from its reference entities.  Hereafter this
	 * feature can be transformed (moved/copied) and the former reference
	 * geometry can be modified without affecting this feature.
	 */
	public boolean Disassociate()
	{
		String cmd = "Toolpath:Disassociate:id=" + Id();
		return ( Portal.Execute( cmd ) );
	}

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Protected methods.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	protected DbFeature( int id )
	{
		Id( id );
	}
}

