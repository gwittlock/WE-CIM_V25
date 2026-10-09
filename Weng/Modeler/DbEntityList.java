
package Weng.Modeler;

import java.util.Vector;

/**
 * The class DbEntityList is essentially a Vector of DbEntity.
 */
public class DbEntityList
{
	public DbEntityList()
	{
		m_entities = new Vector<DbEntity>();
	}
	
	public int Count()
	{
		return m_entities.size();
	}
	
	public DbEntity Get( int indx )
	{
		DbEntity	dbEntity;
		int			count;
		
		count = m_entities.size();
		
		if (indx >= 0 && indx < count)
			dbEntity = ((DbEntity) m_entities.get(indx));
		else
			dbEntity = null;
			
		return dbEntity;
	}
	
	public void Set( int indx, DbEntity dbEntity )
	{
		m_entities.set( indx, dbEntity );
	}
	
	public void Append( DbEntity dbEntity )
	{
		m_entities.add( dbEntity );
	}
	
	/**
	 * A benign flush.
	 */
	public void Flush()
	{
		m_entities.clear();
	}
	
	public DbEntity [] ToArray()
	{
		DbEntity []	array;
		int			count, indx;
		
		count = m_entities.size();
		array = new DbEntity[count];

		for (indx = 0; indx < count; ++ indx)
		{
			array[indx] = (DbEntity) m_entities.get(indx);
		}
		
		return array;
	}
	
	private Vector<DbEntity> m_entities;
}

