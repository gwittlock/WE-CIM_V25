
public class LibTest
{
	static public void main ( String argv[] )
	{
		Msg msg = new Msg();
		
		ErrSys errSys = new ErrSys();

		OutSys OutSys = new OutSys();

		Registry reg = new Registry();

		Clfile clfile = new Clfile();

		Msg.Display( "If no errors, then all's okay." );
	}
}

