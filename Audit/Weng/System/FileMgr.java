
package Weng.System;

/**
 * Use this singleton class to perform file operations.
 */
public class FileMgr
{
	/**
	 * Determines whether the given file exists.
	 * @param fileName fully qualified path of the file in question
	 * @return (true) the file exists / (false) the file does not exist
	 */
	static public native boolean Exists( String fileName );

	/**
	 * Returns the length of the file as number of bytes.
	 */
	static public native int Length( String fileName );

	/**
	 * Renames the source file using the target file name.
	 * @return (true) succeeded / (false) failed
	 */
	static public native boolean Rename( String sourceName, String targetName );

	/**
	 * Copies the source file to the target file.
	 * @param sourceName fully qualified path of the source file
	 * @param targetName fully qualified path of the target file
	 * @return (true) succeeded / (false) failed
	 */
	static public native boolean Copy( String sourceName, String targetName );

	/**
	 * Deletes the named file.
	 * @param fileName fully qualified path of the file in question
	 * @return (true) succeeded / (false) failed
	 */
	static public native boolean Delete( String fileName );

	private FileMgr()
	{
	}

	static
	{
		String libName = "mm2common";

		try
		{
			System.loadLibrary( libName );
		}
		catch (UnsatisfiedLinkError e)
		{
			// TODO: We're in deep trouble if this happens because
			// there will be no method of displaying the error via
			// the ui unless we have the java SWING classes!
			System.out.println( "Fatal Error: class FileMgr failed to load library "+libName );
		}
	}
}

