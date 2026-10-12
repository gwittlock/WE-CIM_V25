
package Weng.Modeler;

import Weng.System.Const;
import Weng.System.Portal;


/**
 * Use this class to create and manage a list of named-attributes.
 */
public class AttribList
{
	/**
	 * Creates an empty AttribList object.
	 */
	public AttribList()
	{
		m_id = ((Portal.Execute( "Attrib:List:New:" )) ? Portal.IntGet( "id" ) : 0);
	}

	/**
	 * Destroys the given AttribList object.
	 * @param attribList the object to be destroyed
	 */
	public boolean Destroy()
	{
		return ( Portal.Execute( "Attrib:List:Destroy: id=" + m_id ) );
	}

	/**
	 * Removes all attributes from this list.
	 */
	public boolean Flush()
	{
		return ( Portal.Execute( "Attrib:List:Flush: id=" + m_id ) );
	}

	/**
	 * Gets the count of attributes in this list.
	 */
	public int Count()
	{
		boolean okay = Portal.Execute( "Attrib:List:Count: id=" + m_id );

		return ((okay) ? Portal.IntGet( "count" ) : 0);
	}

	/**
	 * Gets the named-attribute as an integer value.
	 * @return (Const.UNDEFINED) indicates the named-atribute was not found
	 */
	public int IntGet( String attribName )
	{
		return ((int) DblGet( attribName ));
	}

	/**
	 * Gets the named-attribute as a double value.
	 * @return (Const.UNDEFINED) indicates the named-atribute was not found
	 */
	public double DblGet( String attribName )
	{
		String sval = StrGet( attribName );
		return ((sval == null || sval.length() < 1) ? Const.UNDEFINED : Double.parseDouble( sval ));
	}

	/**
	 * Gets the named-attribute as a string value.
	 * @return (null) indicates the named-atribute was not found
	 */
	public String StrGet( String attribName )
	{
		String result = null;

		String cmd = "Attrib:List:Get:"
					 + " id=" + m_id
					 + ",name=\"" + attribName + "\"";

		if ( Portal.Execute( cmd ) )
			result = Portal.StringGet( "val" );

		return result;
	}

	/**
	 * Sets the integer value named-attribute.
	 */
	public boolean IntSet( String attribName, int value )
	{
		String sval = Integer.toString( value );
		return ( StrSet( attribName, sval ) );
	}

	/**
	 * Sets the double value named-attribute.
	 */
	public boolean DblSet( String attribName, double value )
	{
		String sval = Double.toString( value );
		return ( StrSet( attribName, sval ) );
	}

	/**
	 * Sets the string value named-attribute.
	 */
	public boolean StrSet( String attribName, String value )
	{
		String cmd = "Attrib:List:Set:"
					 + " id=" + m_id
					 + ",name=\"" + attribName + "\""
					 + ",val=\"" + value + "\"";

		return ( Portal.Execute( cmd ) );
	}

	/**
	 * Removes the named-attribute.
	 */
	public boolean Delete( String attribName )
	{
		String cmd = "Attrib:List:Del:"
					 + " id=" + m_id
					 + ",name=\"" + attribName + "\"";

		return ( Portal.Execute( cmd ) );
	}

	/**
	 * Gets the name of the Nth named-attribute.
	 */
	public String Name( int indx )
	{
		String cmd = "Attrib:List:Name:"
					 + " id=" + m_id
					 + ",indx=" + indx;

		return ((Portal.Execute( cmd )) ? Portal.StringGet( "name" ) : "");
	}


	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Private Methods.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	protected void finalize() throws Throwable
	{
		super.finalize();
	}


	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Private Data.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	int m_id = 0;
}
