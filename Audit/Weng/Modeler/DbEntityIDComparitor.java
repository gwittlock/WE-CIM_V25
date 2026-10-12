
package Weng.Modeler;

import java.util.Comparator;


/**
 * The class DbEntityIDComparitor is used to sort arrays
 * of entities on order of increasing entity ID and to
 * search sorted arrays for an entity.
 */
public class DbEntityIDComparitor implements Comparator
{
	public DbEntityIDComparitor()
	{
	}
	
    public int compare( Object o1, Object o2 )
    {
        DbEntity dbEntityA = (DbEntity)o1;
        DbEntity dbEntityB = (DbEntity)o2;
        
        int idA = dbEntityA.Id();
        int idB = dbEntityB.Id();
        
        if (idA < idB)
        	return -1;
        else if (idA > idB)
        	return 1;
        else
        	return 0;
    }
    
    public boolean equals( Object o1, Object o2 )
    {
        DbEntity dbEntityA = (DbEntity)o1;
        DbEntity dbEntityB = (DbEntity)o2;
        
        int idA = dbEntityA.Id();
        int idB = dbEntityB.Id();
        
        return (idA == idB);
    }
}

