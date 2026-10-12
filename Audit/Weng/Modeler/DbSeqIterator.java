
package Weng.Modeler;

import Weng.System.Portal;
import Weng.System.Const;

/**
 * Singleton class.
 */
public class DbSeqIterator
{
	/**
	 */
	static public String Init( int id )
	{
		String msg = null;
		
		if (Portal.Execute( "Seq:Iter_init:id=" + id ) == false)
		{
			msg = "DbSeqIterator::Init() -- failed";
		}
		
		return msg;
	}
	
	/**
	 */
	static public String Next()
	{
		String msg = null;
		
		if (Portal.Execute( "Seq:Iter_next:" ) == false)
		{
			msg = "DbSeqIterator::Next() -- failed";
		}
		
		return msg;
	}
	
	/**
	 */
	static public DbEntity Get()
	{
		DbEntity dbEntity = null;
		
		if (Portal.Execute( "Seq:Iter_get:" ) == true)
		{
			int id = Portal.IntGet("id");
			if (id > 0)
			{
				dbEntity = new DbEntity( id );
			}
		}
		
		return dbEntity;
	}

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Private methods.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	
	private DbSeqIterator()
	{
	}
}

