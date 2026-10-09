
package Weng.Modeler;

import Weng.System.*;
import Weng.Access.*;


/**
 * This singleton class serves as a proxy to the modeler database.
 * Use this class to get entities from the modeler database.
 */
public class CMDB
{
	/**
	 * Returns the path to the current CMDB as store in the Window's registry.
	 */
	static public String Path()
	{
		String key = Registry.REGROOT + "\\ConfigurationManager";
		return ( Registry.StringGet( key, "Database" ) );
	}

	/**
	 * Get the feedrate to be used with the given tool/material combination.
	 * @param materialName eg. the value returned by Model.StringGet("MatCfg")
	 * @param toolType eg. the value returned by clfile.Int("Type_ID")
	 * @param toolDiam eg. the value returned by clfile.Dbl("Kerf")
	 * @return the corresponding feedrate
	 */
	static public double Feedrate(
								String	materialName,
								int		toolType,
								double	toolDiam )
	{
		double feed = 0.;
		
		if (DEBUG) Msg.Display( "CMDB::Feedrate():" + Path() );

		AccessDb db = AccessDb.Create();

		if (db != null)
		{
			AccessTable MatAttrib = db.TableAdd( "Material Attributes" );

			if ( db.Open( Path() ) )
			{
				String sqlStatement;

				AccessQuery query = AccessQuery.Create( db );

				sqlStatement = "SELECT * FROM [Material Attributes] WHERE " 
								+ "(([Description]=\"" + materialName + "\") AND" 
								+ " ([Tool Type ID]=" + toolType + ") AND" 
								+ " ([Diameter Limit]<=" + toolDiam + "))";

				if (DEBUG) Msg.Display( "FeedrateFromCMDB():" + sqlStatement );

				query.Execute( sqlStatement );

				if (query.Count() > 0)
				{
					feed = query.DblGet("Feed");
				}

				AccessQuery.Destroy( query );

				db.Close();
			}

			AccessDb.Destroy( db );
		}

		if (DEBUG) Msg.Display( "CMDB::Feedrate():" + feed );

		return feed;
	}

	private CMDB()
	{
	}

	static private boolean DEBUG = false;;
}

