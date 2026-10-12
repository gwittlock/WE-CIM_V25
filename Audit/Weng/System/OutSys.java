
package Weng.System;

/**
 * Use this singleton class to control file output of a
 * CNC Code Generator.
 */
public class OutSys
{
	/**
	 * Return value for Type().
	 */
	static public final int NONE	=  0;

	/**
	 * Return value for Type().
	 */
	static public final int EDITOR	= -1;

	/**
	 * Return value for Type().
	 */
	static public final int FILE	=  1;

	/**
	 * This objects internal storage buffer for CNC output.  Instead
	 * of directly accessing this buffer, the preferred method is to
	 * use <b>Dump( String )</b>.
	 */
	public String buffer;

	/**
	 * Gets the fully qualified path to the CNC output file that was
	 * specified in the machining applications CNC Export dialog.
	 */
	public native String Path();

	/**
	 * Opens the output file to receive output from this object.  If
	 * ever you're output file is empty, chances are that you did not
	 * make a call to Open().
	 * @return (-1) okay, but the internal buffer will not be written to
	 *         an output file.  This value is returned when you select
	 *         the machining applications View/Editor menu item.<br>
	 *         (0) failure, could not open the output file.<br>
	 *         (1) okay, succeeded in opening the output file.<br>
	 */
	public native int Open();
	public native int OpenForAppend();
	public native int OpenAsBinary();

	/**
	 * Closes the output file.
	 */
	public native void Close();

	/**
	 * Gets the output system type.
	 * @return one of NONE, EDITOR, FILE.
	 */
	public native int Type();

	/**
	 * Writes the contents of the internal buffer the output file (or the
	 * View/Editor window), and the clears the buffer contents.
	 */
	public void Dump( )
	{
		if (buffer.length() > 0)
		{
			if ( IsValid() )
				Export(buffer);
			else
				System.out.println(buffer);
		}
	}

	/**
	 * Writes the given string to the output file (or the
	 * View/Editor window).
	 */
	public void Dump( String block )
	{
		buffer = block;
		Dump();
	}

	private native boolean IsValid( );
	private native int Export( String buffer );

	static
	{
		String libName = "mm2common";

		try
		{
			System.loadLibrary( libName );
		}
		catch (UnsatisfiedLinkError e)
		{
			Msg.Display( "Fatal Error: class OutSys failed to load library "+libName );
		}
	}
}

