
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// TODO: Simplify 'format testing' by making this class a code generator.
// That way, we can use output.Dump() with View/Editor instead of using
// System.out.println() with a DOS shell.
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

import Weng.CodeGen.*;

public class AddrFmtTest
{
	static public void main ( String argv[] )
	{
		DataInit();

		for (int indx = 0; indx < 78; ++indx)
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

			if (result == null)
				System.out.println( "("+indx+") *** NULL" );
			else if (d.result.compareTo( result ) != 0)
				System.out.println( "("+indx+") *** <"+d.result+"> <"+result+">" );
			else
				System.out.println( "("+indx+")     <"+d.result+"> <"+result+">" );
		}
	}

	static private void DataInit()
	{
		m_theData = new AddrFmtTestData[80];

		m_theData[ 0] = new AddrFmtTestData( 0.0, "", "---- Sign Test ----", 0, 0, 0, 0, 0, 0, "0" );
		m_theData[ 1] = new AddrFmtTestData( 0.0, "", "NeverSign", 0, 0, 0, 0, 0, 0, "0" );
		m_theData[ 2] = new AddrFmtTestData( 0.1, "X0.1",   "X", 0, 1, 3, 1, 0, 4, "0" );
		m_theData[ 3] = new AddrFmtTestData(-0.1, "X0.1",   "X", 0, 1, 3, 1, 0, 4, "0" );
		m_theData[ 4] = new AddrFmtTestData( 0.0, "", "AlwaysPlus", 0, 0, 0, 0, 0, 0, "0" );
		m_theData[ 5] = new AddrFmtTestData( 0.1, "X+0.1",  "X", 1, 1, 3, 1, 0, 4, "0" );
		m_theData[ 6] = new AddrFmtTestData(-0.1, "X+0.1",  "X", 1, 1, 3, 1, 0, 4, "0" );
		m_theData[ 7] = new AddrFmtTestData( 0.0, "", "PlusWhenPos", 0, 0, 0, 0, 0, 0, "0" );
		m_theData[ 8] = new AddrFmtTestData( 0.1, "X+0.1",  "X", 2, 1, 3, 1, 0, 4, "0" );
		m_theData[ 9] = new AddrFmtTestData(-0.1, "X0.1",   "X", 2, 1, 3, 1, 0, 4, "0" );
		m_theData[10] = new AddrFmtTestData( 0.0, "", "PlusWhenNeg", 0, 0, 0, 0, 0, 0, "0" );
		m_theData[11] = new AddrFmtTestData( 0.1, "X0.1",   "X", 3, 1, 3, 1, 0, 4, "0" );
		m_theData[12] = new AddrFmtTestData(-0.1, "X+0.1",  "X", 3, 1, 3, 1, 0, 4, "0" );
		m_theData[13] = new AddrFmtTestData( 0.0, "", "MinusWhenPos", 0, 0, 0, 0, 0, 0, "0" );
		m_theData[14] = new AddrFmtTestData( 0.1, "X-0.1",  "X", 4, 1, 3, 1, 0, 4, "0" );
		m_theData[15] = new AddrFmtTestData(-0.1, "X0.1",   "X", 4, 1, 3, 1, 0, 4, "0" );
		m_theData[16] = new AddrFmtTestData( 0.0, "", "MinusWhenNeg", 0, 0, 0, 0, 0, 0, "0" );
		m_theData[17] = new AddrFmtTestData( 0.1, "X0.1",   "X", 5, 1, 3, 1, 0, 4, "0" );
		m_theData[18] = new AddrFmtTestData(-0.1, "X-0.1",  "X", 5, 1, 3, 1, 0, 4, "0" );
		m_theData[19] = new AddrFmtTestData( 0.0, "", "AlwaysMinus", 0, 0, 0, 0, 0, 0, "0" );
		m_theData[20] = new AddrFmtTestData( 0.1, "X-0.1",  "X", 6, 1, 3, 1, 0, 4, "0" );
		m_theData[21] = new AddrFmtTestData(-0.1, "X-0.1",  "X", 6, 1, 3, 1, 0, 4, "0" );
		m_theData[22] = new AddrFmtTestData( 0.0, "", "AlwaysSign", 0, 0, 0, 0, 0, 0, "0" );
		m_theData[23] = new AddrFmtTestData( 0.1, "X+0.1",  "X", 7, 1, 3, 1, 0, 4, "0" );
		m_theData[24] = new AddrFmtTestData(-0.1, "X-0.1",  "X", 7, 1, 3, 1, 0, 4, "0" );
		m_theData[25] = new AddrFmtTestData( 0.0, "", "AlwaysReversed", 0, 0, 0, 0, 0, 0, "0" );
		m_theData[26] = new AddrFmtTestData( 0.1, "X-0.1",  "X", 8, 1, 3, 1, 0, 4, "0" );
		m_theData[27] = new AddrFmtTestData(-0.1, "X+0.1",  "X", 8, 1, 3, 1, 0, 4, "0" );

		// Testing of the decimal is inherent in other tests.
		// m_theData[] = new AddrFmtTestData( 0.0, "", "---- Decimal Test ----", 0, 0, 0, 0, 0, 0, "0" );

		m_theData[28] = new AddrFmtTestData( 0.0,  "", "---- Lead Zeros Test ----", 0, 0, 0, 0, 0, 0, "0" );
		m_theData[29] = new AddrFmtTestData( 0.0,  "", "NeverLeadZero", 0, 0, 0, 0, 0, 0, "0" );
		m_theData[30] = new AddrFmtTestData(10.0,  "X10.",    "X", 5, 0, 3, 1, 0, 4, "0" );
		m_theData[31] = new AddrFmtTestData( 1.0,  "X1.",     "X", 5, 0, 3, 1, 0, 4, "0" );
		m_theData[32] = new AddrFmtTestData( 0.1,  "X.1",     "X", 5, 0, 3, 1, 0, 4, "0" );
		m_theData[33] = new AddrFmtTestData( 0.01, "X.01",    "X", 5, 0, 3, 1, 0, 4, "0" );
		m_theData[34] = new AddrFmtTestData( 0.01, "X1",      "X", 5, 0, 3, 0, 0, 4, "0" );
		m_theData[35] = new AddrFmtTestData( 0.0,  "", "OneLeadZero", 0, 0, 0, 0, 0, 0, "0" );
		m_theData[36] = new AddrFmtTestData(10.0,  "X10.",    "X", 5, 1, 3, 1, 0, 4, "0" );
		m_theData[37] = new AddrFmtTestData( 1.0,  "X1.",     "X", 5, 1, 3, 1, 0, 4, "0" );
		m_theData[38] = new AddrFmtTestData( 0.1,  "X0.1",    "X", 5, 1, 3, 1, 0, 4, "0" );
		m_theData[39] = new AddrFmtTestData( 0.01, "X001",    "X", 5, 1, 3, 0, 0, 4, "0" );
		m_theData[40] = new AddrFmtTestData( 0.0,  "", "AlwaysLeadZero", 0, 0, 0, 0, 0, 0, "0" );
		m_theData[41] = new AddrFmtTestData(10.0,  "X010.",   "X", 5, 2, 3, 1, 0, 4, "0" );
		m_theData[42] = new AddrFmtTestData( 1.0,  "X001.",   "X", 5, 2, 3, 1, 0, 4, "0" );
		m_theData[43] = new AddrFmtTestData( 0.1,  "X0001",   "X", 5, 2, 3, 0, 0, 4, "0" );
		m_theData[44] = new AddrFmtTestData( 0.01, "X000.01", "X", 5, 2, 3, 1, 0, 4, "0" );
		m_theData[45] = new AddrFmtTestData( 0.01, "X00001",  "X", 5, 2, 3, 0, 0, 4, "0" );

		m_theData[46] = new AddrFmtTestData( 0.0, "", "---- Trail Zeros Test ----", 0, 0, 0, 0, 0, 0, "0" );
		m_theData[47] = new AddrFmtTestData( 0.0,  "", "NeverTrailZero", 0, 0, 0, 0, 0, 0, "0" );
		m_theData[48] = new AddrFmtTestData(10.0,  "X10.",    "X", 5, 0, 3, 1, 0, 4, "0" );
		m_theData[49] = new AddrFmtTestData( 1.0,  "X1.",     "X", 5, 0, 3, 1, 0, 4, "0" );
		m_theData[50] = new AddrFmtTestData( 0.1,  "X.1",     "X", 5, 0, 3, 1, 0, 4, "0" );
		m_theData[51] = new AddrFmtTestData( 0.01, "X.01",    "X", 5, 0, 3, 1, 0, 4, "0" );
		m_theData[52] = new AddrFmtTestData( 0.01, "X1",      "X", 5, 0, 3, 0, 0, 4, "0" );
		m_theData[53] = new AddrFmtTestData(25.00001, "X25.", "X", 5, 0, 3, 1, 0, 4, "0" );
		m_theData[54] = new AddrFmtTestData( 0.0,  "", "OneTrailZero", 0, 0, 0, 0, 0, 0, "0" );
		m_theData[55] = new AddrFmtTestData(10.0,  "X10.0",   "X", 5, 1, 3, 1, 1, 4, "0" );
		m_theData[56] = new AddrFmtTestData( 1.0,  "X1.0",    "X", 5, 1, 3, 1, 1, 4, "0" );
		m_theData[57] = new AddrFmtTestData( 0.1,  "X0.1",    "X", 5, 1, 3, 1, 1, 4, "0" );
		m_theData[58] = new AddrFmtTestData( 0.01, "X0.01",   "X", 5, 1, 3, 1, 1, 4, "0" );
		m_theData[59] = new AddrFmtTestData( 0.0,  "", "AlwaysTrailZero", 0, 0, 0, 0, 0, 0, "0" );
		m_theData[60] = new AddrFmtTestData(10.0,  "X10.0000","X", 5, 1, 3, 1, 2, 4, "0" );
		m_theData[61] = new AddrFmtTestData(130.0, "X13000",  "X", 5, 0, 3, 0, 2, 2, "0" );
		m_theData[62] = new AddrFmtTestData( 1.0,  "X1.0000", "X", 5, 1, 3, 1, 2, 4, "0" );
		m_theData[63] = new AddrFmtTestData( 0.1,  "X0.1000", "X", 5, 1, 3, 1, 2, 4, "0" );
		m_theData[64] = new AddrFmtTestData( 0.01, "X0.0100", "X", 5, 1, 3, 1, 2, 4, "0" );
		m_theData[65] = new AddrFmtTestData( 0.01, "X100",    "X", 5, 0, 3, 0, 2, 4, "0" );

		// Testing of the decimal is inherent in other tests?
		// m_theData[] = new AddrFmtTestData( 0.0, "", "---- Abscissa Test ----", 0, 0, 0, 0, 0, 0, "0" );

		// Testing of the decimal is inherent in other tests?
		// m_theData[] = new AddrFmtTestData( 0.0, "", "---- Mantissa Test ----", 0, 0, 0, 0, 0, 0, "0" );

		m_theData[66] = new AddrFmtTestData( 0.0, "", "---- Zeros Test ----", 0, 0, 0, 0, 0, 0, "0" );
		m_theData[67] = new AddrFmtTestData( 0.0, "X0.0",   "X", 0, 0, 0, 0, 0, 0, "0.0" );
		m_theData[68] = new AddrFmtTestData( 0.0, "X+0.0",  "X", 0, 0, 0, 0, 0, 0, "+0.0" );
		m_theData[69] = new AddrFmtTestData( 0.0, "X-0.0",  "X", 0, 0, 0, 0, 0, 0, "-0.0" );
		m_theData[70] = new AddrFmtTestData( 0.0, "X0.",    "X", 0, 0, 0, 0, 0, 0, "0." );
		m_theData[71] = new AddrFmtTestData( 0.0, "X.0",    "X", 0, 0, 0, 0, 0, 0, ".0" );
		m_theData[72] = new AddrFmtTestData( 0.0, "X0",     "X", 0, 0, 0, 0, 0, 0, "0" );
		m_theData[73] = new AddrFmtTestData( 0.0, "X000000","X", 0, 0, 0, 0, 0, 0, "000000" );

		// Special test because Sun's Double.toString() formats numbers smaller
		// than 1.e-3 using scientific notation.  This forces us to take extra
		// steps to get such numbers properly formatted.
		m_theData[74] = new AddrFmtTestData( 0.0, "", "---- Special Tests ----", 0, 0, 0, 0, 0, 0, "0" );
		m_theData[75] = new AddrFmtTestData( 0.00012, "X.0001","X", 5, 0, 3, 1, 0, 4, "0" );

		// Special test for proper rounding (found by Bill Oliver)
		m_theData[76] = new AddrFmtTestData( 28.2455499999, "X28.2455","X", 5, 0, 3, 1, 0, 4, "0" );
		m_theData[77] = new AddrFmtTestData( 28.24555, "X28.2456","X", 5, 0, 3, 1, 0, 4, "0" );

		m_theData[78] = new AddrFmtTestData( 99999, "", "", 0, 0, 0, 0, 0, 0, "0" );
	}

	static private AddrFmtTestData m_theData[];
}

