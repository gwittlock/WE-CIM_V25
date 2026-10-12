
package Weng.Access;

import Weng.System.*;


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
/**
 * An object of the class AccessDb serves as a proxy to
 * a Microsoft Access database.  Special low-level
 * implementation issues require this class to be
 * implemented as a factory.  That is, the static method
 * Create() is used to construct an AccessDb object, and
 * the static method Destroy() is used to remove an
 * AccessDb object.
 */
public class AccessDb
{
	/**
	 * Create an AccessDb Object.
	 */
	static public AccessDb Create()
	{
		return ( new AccessDb() );
	}

	/**
	 * Destroy the an AccessDb Object.
	 * @param db the object to be destroyed
	 */
	static public void Destroy( AccessDb db )
	{
		if (db == null)
			return;

		db.Close();

		String cmd = "Db:DbDestroy: id=" + db.Id();
		Portal.Execute( cmd );

		db.m_path = null;
		db.m_id = 0;
	}

	/**
	 * This object will open the specified database.  NOTE: Prior
	 * to opening a database, use the method TableAdd() to add
	 * names of the tables that this object will access.
	 * @param path the fully qualified path to a database
	 */
	public boolean Open( String path )
	{
		String cmd = "Db:DbOpen:"
					 + " id=" + m_id
					 + ",file=\"" + path + "\"";

		boolean okay = Portal.Execute( cmd );

		if ( okay )
		{
			int bool = Portal.IntGet( "bool" );
			okay = (bool != 0);
		}

		return okay;
	}

	/**
	 * Close the database that is being accessed by this object.
	 */
	public void Close()
	{
		if (m_id > 0)
		{
			String cmd = "Db:DbClose: id=" + m_id;
			Portal.Execute( cmd );
		}
	}

	/**
	 * Add the name of a database table to be accessed by this object.
	 * @param tableName the name of a database table to add to this object
	 */
	public AccessTable TableAdd( String tableName )
	{
		AccessTable table = null;

		String cmd = "Db:DbTable:"
					 + " id=" + m_id
					 + ",name=\"" + tableName + "\"";

		if ( Portal.Execute( cmd ) )
		{
			int tableNo = Portal.IntGet( "table" );

			if (tableNo >= 0)
				table = new AccessTable( this, tableNo );
		}

		return table;
	}


	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Protected methods
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	protected AccessDb()
	{
		m_path = "";

		boolean okay = Portal.Execute( "Db:DbCreate:" );

		m_id = ((okay) ? Portal.IntGet( "id" ) : 0);
	}

	protected int Id()
	{
		return m_id;
	}

	protected void finalize() throws Throwable
	{
		m_path = null;

		super.finalize();
	}

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Private methods & data
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	private String m_path = new String( "" );
	private int m_id = 0;
}
