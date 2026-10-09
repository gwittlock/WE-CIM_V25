
package Weng.System;

import java.io.OutputStream;

/**
 * Do <b>not</b> attempt to use.
 */
public class ErrSys extends OutputStream
{
	public String buffer;

	public void write( int b )
	{
		char c = ((char) b);

		if (c < ' ' || c > '}')
		{
			Dump();
		}
		else
		{
			buffer += c;
		}
	}

	public void Dump( )
	{
		if (buffer.length() > 0)
		{
// System.out.println(buffer);
			if ( IsValid() )
				Export(buffer);
			else
				System.out.println(buffer);

			buffer = "";
		}
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
			Msg.Display( "Fatal Error: class ErrSys failed to load library "+libName );
// System.out.println( "Fatal Error: class ErrSys failed to load library "+libName );
		}
	}
}

