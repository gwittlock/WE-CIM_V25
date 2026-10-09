
public class Test
{
	static public void main ( String argv[] )
	{
		AddrFmt fmt = new AddrFmt( "T", 0, 0, 3, 0, 0, 0, "0" );

		String result = fmt.Format( 20 );

		System.out.println( "Result <"+result+">" );
	}
}

