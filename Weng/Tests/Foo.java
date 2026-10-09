
// import sun.tools.javac.Main;


public class Foo
{
	public static void main( String[] args )
	{
		int i = 0;
		int[] j = new int[2];

		j[0] = 0;
		j[1] = 1;

		Bar( i );
		Bar( j );

		System.out.println( "i <"+i+">" );
		System.out.println( "j[0] <"+j[0]+">" );
		System.out.println( "j[1] <"+j[1]+">" );
	}

	static public void Bar( int i )
	{
		++i;
	}

	static public void Bar( int[2] j )
	{
		++j[0];
		++j[1];
	}

}


