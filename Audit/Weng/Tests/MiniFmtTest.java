
import Weng.CodeGen.*;

public class MiniFmtTest
{
	static public void main ( String argv[] )
	{
		DataInit();

		int indx = 0;

		while (m_theData[indx].value < 99999)
		{
			AddrFmtTestData d = m_theData[indx];

			AddrFmt fmt = new AddrFmt(
									d.addr,
									d.nzSign,
									d.nzLeadZeros,
									d.nzAbscissa,
									d.nzDecimal,
									d.nzTrailZeros,
									d.nzMantissa,
									d.zFormat );

			String result = fmt.Format(d.value);

			if (d.result.compareTo( result ) != 0)
				System.out.println( "("+indx+") *** <"+d.result+"> <"+result+">" );
			else
				System.out.println( "("+indx+")     <"+d.result+"> <"+result+">" );

			++indx;
		}
	}

	static private void DataInit()
	{
		m_theData = new AddrFmtTestData[80];

		// m_theData[0] = new AddrFmtTestData( 0.000122998, "X.0001",  "X", 5, 0, 3, 1, 0, 4, "0" );
		m_theData[0] = new AddrFmtTestData( 0.0001, "X.0001",  "X", 5, 0, 3, 1, 0, 4, "0" );
		m_theData[1] = new AddrFmtTestData( 99999, "", "", 0, 0, 0, 0, 0, 0, "0" );

//		m_theData[0] = new AddrFmtTestData( 0.009999999, "X.01",    "X", 5, 0, 5, 1, 0, 2, ".0" );
//		m_theData[1] = new AddrFmtTestData( -0.009999999, "X-.01",    "X", 5, 0, 5, 1, 0, 2, ".0" );
//		m_theData[2] = new AddrFmtTestData( 1.0, "X1.",    "X", 5, 0, 5, 1, 0, 2, ".0" );
//		m_theData[3] = new AddrFmtTestData( -1.0, "X-1.",    "X", 5, 0, 5, 1, 0, 2, ".0" );
//		m_theData[4] = new AddrFmtTestData( 130.0, "X13000",  "X", 5, 0, 4, 0, 2, 2, "0" );
//		m_theData[5] = new AddrFmtTestData( 99999, "", "", 0, 0, 0, 0, 0, 0, "0" );

//		m_theData[0] = new AddrFmtTestData( 0.01, "X0.01",     "X", 5, 1, 3, 1, 0, 4, "0" );
//		m_theData[1] = new AddrFmtTestData( 25.00001, "X25.",  "X", 5, 0, 3, 1, 0, 4, "0" );
//		m_theData[2] = new AddrFmtTestData( 25.000001, "X25.",  "X", 5, 0, 3, 1, 0, 4, "0" );
//		m_theData[3] = new AddrFmtTestData( 0.0, "X.0",    "X", 0, 0, 0, 0, 0, 0, ".0" );
//		m_theData[4] = new AddrFmtTestData( 0.009999999, "X.01",    "X", 5, 0, 5, 1, 0, 2, ".0" );

//		m_theData[5] = new AddrFmtTestData( 99999, "", "", 0, 0, 0, 0, 0, 0, "0" );
	}

	static private AddrFmtTestData m_theData[];
}

