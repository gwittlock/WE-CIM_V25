
package Weng.Modeler;

import Weng.System.Portal;
import Weng.System.Const;

public class DbPattern extends DbContainer {

    public DbPattern()
    {
        String cmd = "Pattern:Create:";
        if ( Portal.Execute(cmd) )
        {
            int id = Portal.IntGet("id");
            Id(id);
        }
    }

    public static DbPattern DbPattern( DbEntity dbentity )
    {
        int id = dbentity.Id();
        if(id <= 0)
            return null;
            
        if(dbentity.Type() != Const.PATTERN)
            return null;
        else
            return new DbPattern(id);
    }

    protected DbPattern( int id )
    {
        Id(id);
    }
}