
package Weng.Access;

import Weng.System.*;


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
/**
 * An object of the class AccessQuery serves as a proxy to
 * a Microsoft Access database queries.  Special low-level
 * implementation issues require this class to be
 * implemented as a factory.  That is, the static method
 * Create() is used to construct an AccessQuery object,
 * and the static method Destroy() is used to remove an
 * AccessQuery object.
 */
public class AccessQuery
{
	/**
	 * Create an AccessQuery Object.
	 */
	static public AccessQuery Create( AccessDb db )
	{
		return ((db == null) ? null : new AccessQuery( db ));
	}

	/**
	 * Destroy the an AccessQuery Object.
	 * @param query the object to be destroyed
	 */
	static public void Destroy( AccessQuery query )
	{
		if (query == null)
			return;

		String cmd = "Db:QueryDestroy: id=" + query.Id();
		Portal.Execute( cmd );
	}

	/**
	 * Execute an SQL query
	 * @return the count of matching records.
	 */
	public int Execute( String sqlQuery )
	{
		String cmd = "Db:QueryExecute:"
					 + " id=" + m_id
					 + ",dbid=" + m_db.Id()
					 + ",sql='" + sqlQuery + "'";

		boolean okay = Portal.Execute( cmd );

		return (( okay ) ? Portal.IntGet( "count" ) : 0);
	}

	/**
	 * @return the count of matching records.
	 */
	public int Count()
	{
		String cmd = "Db:QueryCount: id=" + m_id;

		boolean okay = Portal.Execute( cmd );

		return (( okay ) ? Portal.IntGet( "count" ) : 0);
	}

	/**
	 * (-) backward one record
	 * (0) to start
	 * (+) forward one record
	 */
	public boolean MoveIncr( int delta )
	{
		String cmd = "Db:QueryMoveIncr:"
					 + " id=" + m_id
					 + ",delta=" + delta;

		boolean okay = Portal.Execute( cmd );

		return (( okay ) ? true : (Portal.IntGet( "bool" ) > 0));
	}

	/**
	 * @return (Const.UNDEFINED) failure / (else) the value
	 */
	public int IntGet( String fieldName )
	{
		String cmd = "Db:QueryIntGet:"
					 + " id=" + m_id
					 + ",field=\"" + fieldName + "\"";

		boolean okay = Portal.Execute( cmd );

		return (( okay ) ? Portal.IntGet( "val" ) : (int) Const.UNDEFINED);
	}

	/**
	 * @return (Const.UNDEFINED) failure / (else) the value
	 */
	public double DblGet( String fieldName )
	{
		String cmd = "Db:QueryDblGet:"
					 + " id=" + m_id
					 + ",field=\"" + fieldName + "\"";

		boolean okay = Portal.Execute( cmd );

		return (( okay ) ? Portal.DoubleGet( "val" ) : Const.UNDEFINED);
	}

	/**
	 * @return (null) failure / (else) the value
	 */
	public String StrGet( String fieldName )
	{
		String cmd = "Db:QueryStrGet:"
					 + " id=" + m_id
					 + ",field=\"" + fieldName + "\"";

		boolean okay = Portal.Execute( cmd );

		return (( okay ) ? Portal.StringGet( "val" ) : null);
	}


	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Protected methods
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	protected AccessQuery( AccessDb db )
	{
		String cmd = "Db:QueryCreate: db=" + db.Id();

		boolean okay = Portal.Execute( cmd );

		if ( okay )
		{
			m_id = Portal.IntGet( "id" );
			m_db = db;
		}
		else
		{
			m_id = 0;
			m_db = null;
		}
	}

	protected int Id()
	{
		return m_id;
	}

	protected void finalize() throws Throwable
	{
		m_id = 0;
		m_db = null;

		super.finalize();
	}

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Private methods & data
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	private int m_id = 0;
	private AccessDb m_db = null;
}
