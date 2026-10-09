
package Weng.System;

import java.lang.Exception;
import java.io.PrintStream;

public class ExceptionPrinter
{
	static public void StackTracePrint( Exception e )
	{
		ErrSys errSys = new ErrSys();
		PrintStream printStream = new PrintStream( errSys );

		e.printStackTrace( printStream );
	}

	private ExceptionPrinter()
	{
	}
}

