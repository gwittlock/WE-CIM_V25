
package Weng.Access;

import Weng.System.*;


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
/**
 * An object of the class AccessTable serves as a proxy to
 * a Microsoft Access database table.
 */
public class AccessTable
{
	/**
	 * Gets the count of records in this table.
	 */
	public int Count()
	{
		String cmd = "Db:DbCount:"
					 + " id=" + m_db.Id()
					 + ",table=" + m_tableNo;

		boolean okay = Portal.Execute( cmd );

		return (( okay ) ? Portal.IntGet( "count" ) : -1);
	}

	/**
	 * Gets the current record number.
	 */
	public int Curr()
	{
		return m_currIndx;
	}

	/**
	 * Move to a known record.
	 * @return (-1) failure / (else) current record number
	 */
	public int MoveAbs( int recordNumber )
	{
		if (recordNumber < 0 || recordNumber >= Count())
			return 0;

		String cmd = "Db:DbMoveAbs:"
					 + " id=" + m_db.Id()
					 + ",table=" + m_tableNo
					 + ",recno=" + recordNumber;

		boolean okay = Portal.Execute( cmd );

		m_currIndx = (( okay ) ? recordNumber : -1);

		return m_currIndx;
	}

	/**
	 * Move incrementally a given number of records.
	 * @return (-1) failure / (else) current record number
	 */
	public int MoveIncr( int numberOfRecords )
	{
		if (numberOfRecords == 0)
			return m_currIndx;

		int recNo = m_currIndx + numberOfRecords;

		if (recNo < 0 || recNo >= Count())
			return -1;

		String cmd = "Db:DbMoveIncr:"
					 + " id=" + m_db.Id()
					 + ",table=" + m_tableNo
					 + ",delta=" + numberOfRecords;

		boolean okay = Portal.Execute( cmd );

		m_currIndx = recNo;

		return m_currIndx;
	}

	/**
	 * Move to the first record whose value matches
	 * the given fieldValue.
	 * @param fieldName the  name of a database field
	 * @param fieldValue the value for which you are searching
	 * @return (-1) failure / (else) current record number
	 */
	public int Move( String fieldName, String fieldValue )
	{
		String cmd = "Db:DbMove:"
					 + " id=" + m_db.Id()
					 + ",table=" + m_tableNo
					 + ",field=\"" + fieldName + "\""
					 + ",value=\"" + fieldValue + "\"";

		boolean okay = Portal.Execute( cmd );

		int recNo = (( okay ) ? Portal.IntGet( "recno" ) : -1);

		if (recNo < 0)
			MoveAbs( m_currIndx );
		else
			m_currIndx = recNo;

		return recNo;
	}

	/**
	 * Gets the value of a field in the current record.
	 * @return (Const.UNDEFINED) failure / (else) the value
	 */
	public int IntGet( String fieldName )
	{
		String cmd = "Db:DbIntGet:"
					 + " id=" + m_db.Id()
					 + ",table=" + m_tableNo
					 + ",field=\"" + fieldName + "\"";

		boolean okay = Portal.Execute( cmd );

		return (( okay ) ? Portal.IntGet( "val" ) : (int)Const.UNDEFINED);
	}

	/**
	 * Gets the value of a field in the current record.
	 * @return (Const.UNDEFINED) failure / (else) the value
	 */
	public double DblGet( String fieldName )
	{
		String cmd = "Db:DbDblGet:"
					 + " id=" + m_db.Id()
					 + ",table=" + m_tableNo
					 + ",field=\"" + fieldName + "\"";

		boolean okay = Portal.Execute( cmd );

		return (( okay ) ? Portal.DoubleGet( "val" ) : Const.UNDEFINED);
	}

	/**
	 * Gets the value of a field in the current record.
	 * @return (null) failure / (else) the value
	 */
	public String StrGet( String fieldName )
	{
		String cmd = "Db:DbStrGet:"
					 + " id=" + m_db.Id()
					 + ",table=" + m_tableNo
					 + ",field=\"" + fieldName + "\"";

		boolean okay = Portal.Execute( cmd );

		return (( okay ) ? Portal.StringGet( "val" ) : null);
	}


	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Protected methods & data
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	// This constructor should only be used by class AccessDb.
	protected AccessTable( AccessDb accessDb, int tableNo )
	{
		m_db = accessDb;
		m_tableNo = tableNo;
		m_currIndx = 0;
	}


	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Private methods & data
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	private AccessDb m_db  = null;
	private int m_tableNo  = -1;
	private int m_currIndx = -1;
}
