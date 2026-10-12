
package Weng.System;

/**
 * Use this singleton class to execute the machining applications
 * Portal commands.  All of Building Blocks Inc. java application
 * extension classes must use these class methods to execute system
 * functionality.
 */
public class Portal
{
	/**
	 * Executes the given Portal command.
	 */
	static public native boolean Execute( String cmd );

	/**
	 * Gets the resulting named-attribute as an integer value.
	 */
	static public native int IntGet( String paramName );

	/**
	 * Gets the resulting named-attribute as a double value.
	 */
	static public native double DoubleGet( String paramName );

	/**
	 * Gets the resulting named-attribute as a string value.
	 */
	static public native String StringGet( String paramName );

	private Portal()
	{
	}

	static
	{
		String libName = "mm2portal";

		try
		{
			System.loadLibrary( libName );
		}
		catch (UnsatisfiedLinkError e)
		{
			System.out.println( "Fatal Error: class DbEntity failed to load library "+libName );
		}
	}
}

