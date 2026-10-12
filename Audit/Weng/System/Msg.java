
package Weng.System;

/**
 * Use this singleton class to issue messages.  Typically used
 * to help debug Code Generator and Macro java files.
 */
public class Msg
{
	/**
	 * Displays the given message on a simple OK button dialog.
	 */
	static public void Display( String msg )
	{
		if (msg.length() > 0)
		{
			if ( IsMessageActive() )
				MessageMsg( msg );
			else
				System.out.println( msg );
		}
	}

	// For internal use only.
	// Conditionally displays the given message on a simple OK button
	// dialog, based upon a registry entry.
	// Set via the registry entry ..\Machining\Java\Debug
	static public void Debug( String msg )
	{
		if (msg.length() > 0)
		{
			if ( IsDebugActive() )
				DebugMsg( msg );
			else
				System.out.println( msg );
		}
	}

	/**
	 * To be used when diagnostic messages should appear, but don't.
	 */
	static public void DiagnosticsEnable( boolean enable )
	{
		// WARN_USER = 1, DIAGNOSTIC = 3
		Portal.Execute( "Admin:Errors:level=" + ((enable) ? 3 : 1) );
	}

	static public void Diagnostic( String msg )
	{
		DiagnosticMsg( msg );
	}
	
	static private native boolean IsMessageActive( );
	static private native void MessageMsg( String msg );

	static private native boolean IsDebugActive( );
	static private native void DebugMsg( String msg );

	static private native void DiagnosticMsg( String msg );

	private Msg()
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
			System.out.println( "Fatal Error: class ErrSys failed to load library "+libName );
		}
	}
}

