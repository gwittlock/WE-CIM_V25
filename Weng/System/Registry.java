
package Weng.System;

import Weng.System.Const;
import Weng.System.Portal;


/**
 * Use this singleton class to access Microsoft Windows registry information.
 */
public class Registry
{
	/**
	 * The product root key in the Windows Registry.
	 */
	static public final String REGROOT	= "SOFTWARE\\WE-CIM\\24.0";
	// static public final String REGROOT	= "SOFTWARE\\WE-CIM\\21.0";
	// static public final String REGROOT	= "SOFTWARE\\WE-CIM\\20.0";
	// static public final String REGROOT	= "SOFTWARE\\WE-CIM\\19.0";
	// static public final String REGROOT	= "SOFTWARE\\CCS-Fab\\1.0";

	/**
	 * Gets the fully qualified file path to a custom product database.
	 * @param productName the name of the product as seen in the Windows Registry
	 * eg. SOFTWARE\WE-CIM\CustomProduct\<b>PanelFront<\b>\Database
	 */
	static public String CustDbPath( String productName )
	{
		String key = REGROOT + "\\CustomProduct\\" + productName;
		return ( StringGet( key, "Database" ) );
	}

	/**
	 * Gets the value of the named registry entry as an integer value.
	 * @param key Microsoft registry key (folder as seen via Regedit.exe)
	 * @param subkey the subkey name (use "" for the default subkey)
	 */
	static public int IntGet( String key, String subkey )
	{
		String sval = StringGet( key, subkey );
		return ((sval == null) ? (int) Const.UNDEFINED : Integer.parseInt( sval ) );
	}

	/**
	 * Sets the value of the named registry entry as a string value.
	 * @param key Microsoft registry key (folder as seen via Regedit.exe)
	 * @param subkey the subkey name (use "" for the default subkey)
	 * @param val the integer value to be recorded in the registry
	 */
	static public void IntPut( String key, String subkey, int val )
	{
		Integer ival = new Integer( val );
		StringPut( key, subkey, Integer.toString( val ) );
	}

	/**
	 * Gets the value of the named registry entry as a double value.
	 * @param key Microsoft registry key (folder as seen via Regedit.exe)
	 * @param subkey the subkey name (use "" for the default subkey)
	 */
	static public double DoubleGet( String key, String subkey )
	{
		String sval = StringGet( key, subkey );
		return ((sval == null) ? Const.UNDEFINED : Double.parseDouble( sval ) );
	}

	/**
	 * Sets the value of the named registry entry as a string value.
	 * @param key Microsoft registry key (folder as seen via Regedit.exe)
	 * @param subkey the subkey name (use "" for the default subkey)
	 * @param val the double value to be recorded in the registry
	 */
	static public void DoublePut( String key, String subkey, double val )
	{
		StringPut( key, subkey, Double.toString( val ) );
	}

	/**
	 * Gets the value of the named registry entry as a string value.
	 * @param key Microsoft registry key (folder as seen via Regedit.exe)
	 * @param subkey the subkey name (use "" for the default subkey)
	 */
	static public String StringGet( String key, String subkey )
	{
		String result = null;

		String cmd = "Admin:RegGet:" +
					 " key=\"" + key +"\"" +
					 ",subkey=\"" + subkey + "\"";

		boolean okay = Portal.Execute( cmd );

		if ( okay )
			result = Portal.StringGet( "val" );

		return result;
	}

	/**
	 * Sets the value of the named registry entry as a string value.
	 * @param key Microsoft registry key (folder as seen via Regedit.exe)
	 * @param subkey the subkey name (use "" for the default subkey)
	 * @param val the string value to be recorded in the registry
	 */
	static public void StringPut( String key, String subkey, String val )
	{
		String cmd = "Admin:RegPut:" +
					 " key=\"" + key + "\"" +
					 ",subkey=\"" + subkey + "\"" +
					 ",val=\"" + val + "\"";

		Portal.Execute( cmd );
	}

	private Registry()
	{
	}
}

