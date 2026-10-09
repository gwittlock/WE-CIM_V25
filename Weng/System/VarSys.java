
package Weng.System;


/**
 * Use this singleton class to manage system attributes.
 */
public class VarSys
{
	/**
	 * Gets the named-attribute as an integer value.
	 * @return (Const.UNDEFINED) indicates the named-atribute was not found
	 */
	static public int IntGet( String attribName )
	{
		double val = DblGet( attribName );
		return ((int) val);
	}

	/**
	 * Gets the named-attribute as an integer value.
	 * @return (found) the defined value / (not found) the default value.
	 */
	static public int IntGet( String attribName, int defaultValue )
	{
		double val = DblGet( attribName );
		return ((int) ((val >= Const.UNDEFINED) ? defaultValue : val));
	}

	/**
	 * Gets the named-attribute as a double value.
	 * @return (Const.UNDEFINED) indicates the named-atribute was not found
	 */
	static public double DblGet( String attribName )
	{
		String sval = StrGet( attribName );
		return ((sval.length() > 0) ? Double.parseDouble( sval ) : Const.UNDEFINED);
	}

	/**
	 * Gets the named-attribute as an integer value.
	 * @return (found) the defined value / (not found) the default value.
	 */
	static public double DblGet( String attribName, double defaultValue )
	{
		double val = DblGet( attribName );
		return ((val >= Const.UNDEFINED) ? defaultValue : val);
	}

	/**
	 * Gets the named-attribute as a string value.
	 * @return (null) indicates the named-atribute was not found
	 */
	static public String StrGet( String attribName )
	{
		String result = null;

		String cmd = "Admin:VarSys:StrGet: name=\"" + attribName + "\"";

		if ( Portal.Execute( cmd ) )
			result = Portal.StringGet( "val" );

		return result;
	}

	/**
	 * Gets the named-attribute as an integer value.
	 * @return (found) the defined value / (not found) the default value.
	 */
	static public String StrGet( String attribName, String defaultValue )
	{
		String sval = StrGet( attribName );

		if (sval.length() <= 0)
			sval = defaultValue;

		return sval;
	}

	/**
	 * Sets the integer value named-attribute.
	 */
	static public boolean IntSet( String attribName, int value )
	{
		String sval = Integer.toString( value );
		return ( StrSet( attribName, sval ) );
	}

	/**
	 * Sets the double value named-attribute.
	 */
	static public boolean DblSet( String attribName, double value )
	{
		String sval = Double.toString( value );
		return ( StrSet( attribName, sval ) );
	}

	/**
	 * Sets the string value named-attribute.
	 */
	static public boolean StrSet( String attribName, String value )
	{
		String cmd = "Admin:VarSys:StrPut:"
					 + " name=\"" + attribName + "\""
					 + ",val=\"" + value + "\"";

		return ( Portal.Execute( cmd ) );
	}

	/**
	 * Removes all attributes from this list.
	 */
	static public boolean Flush()
	{
		return ( Portal.Execute( "Admin:VarSys:Flush:" ) );
	}

	/**
	 * Gets the count of attributes in this list.
	 */
	static public int Count()
	{
		boolean okay = Portal.Execute( "Admin:VarSys:Count:" );

		return ((okay) ? Portal.IntGet( "count" ) : 0);
	}

	/**
	 * Removes the named-attribute.
	 */
	static public boolean Delete( String attribName )
	{
		String cmd = "Admin:VarSys:Del: name=\"" + attribName + "\"";

		return ( Portal.Execute( cmd ) );
	}

	/**
	 * Gets the name of the Nth named-attribute.
	 */
	static public String Name( int indx )
	{
		String cmd = "Admin:VarSys:Name: indx=" + indx;

		return ((Portal.Execute( cmd )) ? Portal.StringGet( "name" ) : "");
	}


	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Private Methods.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	protected void finalize() throws Throwable
	{
		super.finalize();
	}

	private VarSys()
	{
	}
}
