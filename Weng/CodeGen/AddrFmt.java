
package Weng.CodeGen;

import java.text.NumberFormat;
import java.text.DecimalFormat;
import java.text.FieldPosition;

import Weng.System.Msg;

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

public class AddrFmt
{
	// enum ESign
	/** NeverSign = 0 */
	static public final int NeverSign       = 0;
	/** AlwaysPlus = 1 */
	static public final int AlwaysPlus      = 1;
	/** PlusWhenPos = 2 */
	static public final int PlusWhenPos     = 2;
	/** PlusWhenNeg = 3 */
	static public final int PlusWhenNeg     = 3;
	/** MinusWhenPos = 4 */
	static public final int MinusWhenPos    = 4;
	/** MinusWhenNeg = 5 */
	static public final int MinusWhenNeg    = 5;
	/** AlwaysMinus = 6 */
	static public final int AlwaysMinus     = 6;
	/** AlwaysSign = 7 */
	static public final int AlwaysSign      = 7;
	/** AlwaysReversed = 8 */
	static public final int AlwaysReversed  = 8;

	// enum ELeadZeros
	/** NeverLeadZero = 0 */
	static public final int NeverLeadZero   = 0;
	/** OneLeadZero = 1 */
	static public final int OneLeadZero     = 1;
	/** AlwaysLeadZero = 2 */
	static public final int AlwaysLeadZero  = 2;

	// enum EDecimal
	/** NeverDecimal = 0 */
	static public final int NeverDecimal    = 0;
	/** AlwaysDecimal = 1 */
	static public final int AlwaysDecimal   = 1;
	/** AlwaysComma = 2 */
	static public final int AlwaysComma     = 2;
	/** DecimalWhenNEM = 3 */
	static public final int DecimalWhenNEM  = 3;  // NEM: Non-Empty Mantissa
	/** CommaWhenNEM = 4 */
	static public final int CommaWhenNEM    = 4;  // NEM: Non-Empty Mantissa

	// enum ETrailZeros
	/** NeverTrailZero = 0 */
	static public final int NeverTrailZero  = 0;
	/** OneTrailZero = 1 */
	static public final int OneTrailZero    = 1;
	/** AlwaysTrailZero = 2 */
	static public final int AlwaysTrailZero = 2;


	public AddrFmt(
					String addr,

					int  nzSign,
					int  nzLeadZeros,
					int  nzAbscissa,
					int  nzDecimal,
					int  nzTrailZeros,
					int  nzMantissa,

					String zFormat )
	{
		m_addr = addr;
		m_nzSign = nzSign;
		m_nzLeadZeros = nzLeadZeros;
		m_nzAbscissa = nzAbscissa;
		m_nzDecimal = nzDecimal;
		m_nzTrailZeros = nzTrailZeros;
		m_nzMantissa = nzMantissa;
		m_zFormat = zFormat;

		m_tolerance = TolCalc( nzAbscissa, nzMantissa );
	}
		
	public double Tolerance()
	{
		return m_tolerance;
	}

	public String Format( double value )
	{
		m_value = value;

		// Cause 'near' numbers to round.
		// NOTE: (1.0 / m_tolerance) is not quite what you might expect.
		double invtol = 1.0 / m_tolerance + 1.e-9;
		double scaled = (Math.abs(m_value) * invtol) + 0.5;

		m_absValue = m_tolerance * ((int) scaled);

		if (m_absValue < m_tolerance)
		{
			return ZeroFormat();
		}
		else
		{
			return NonZeroFormat();
		}
	}

	private String ZeroFormat()
	{
		return Addr() + m_zFormat;
	}

	private String NonZeroFormat()
	{
		m_string = Double.toString( m_absValue );

		if (m_absValue < 1.e-3)
		{
			// Per Sun's documentation regarding 'static String Double.toString()'
			// any value less than 1.e-3 is formatted using scientific notation.

			int len = m_string.length();
			int nZeros = Character.digit( m_string.charAt( len-1 ), 10 );
			String tmp = "." + Zeros( nZeros-1 );
			tmp += m_string.charAt(0);
			tmp += m_string.substring( 2, len-3 );
			m_string = tmp;
		}

		if ((m_nzLeadZeros == NeverLeadZero) && (m_absValue < 1.0))
		{
			// Strip the leading zero from a string like '0.01'
			// Leading zeros are conditionally added to the
			// the formatted number later in the process.

			if (m_string.charAt(0) == '0')		
				m_string = m_string.substring(1);
		}

		m_decPos = m_string.indexOf( '.' );

		if (m_decPos < 0)
		{
			// We must have a defined decimal position!
			// This conditional may well be an artifact of
			// the previous incarnation of the formatter.

			m_decPos = m_string.length();
			m_string += ".";
		}

		if (m_nzMantissa == AlwaysTrailZero)
		{
			// Arrrrgh!  This kludge pads the end of m_string to
			// prevent NonZeroMantissa() from crashing under
			// certain circumstances.  For example, before this
			// kludge was introduced, NonZeroMatissa() would
			// crash when 1) m_string == "2.8" and
			// 2) m_nzMantissa == AlwaysTrailZero and
			// 3) m_nzTrailZeros == 2
			
			m_string += "000000";
		}
		
		// System.out.println( "Raw formatted value <"+m_string+">" );

		String buf = Addr();
		buf += NonZeroSign();
		buf += NonZeroAbscissa();
		buf += Decimal( m_nzDecimal );
		buf += NonZeroMantissa();

		return buf;
	}

	public String Addr()
	{
		return m_addr;
	}

	public void Addr( String addr )
	{
		m_addr = addr;
	}
	
	// Essentially returns the last formatted value as rounded
	// using the tolerance of this register.
	public double RoundedValue()
	{
		return ((m_value < 0) ? -m_absValue : m_absValue);
	}

	// For the incremental output case.
	public double RoundedValue( double value )
	{
		// Cause 'near' numbers to round.
		// NOTE: (1.0 / m_tolerance) is not quite what you might expect.
		double invtol = 1.0 / m_tolerance + 1.e-9;
		double scaled = (Math.abs(value) * invtol) + 0.5;

		value = m_tolerance * ((int) scaled);
		
		return ((value < 0) ? -value : value);
	}

	private String Zeros( int nzeros )
	{
		String buf = new String();

		int count = 0;

		while (count < nzeros)
		{
			buf += '0';
			++count;
		}

		return buf;
	}

	private String Decimal( int decimal )
	{
		switch (decimal)
		{
		case AlwaysDecimal:  return ".";
		case AlwaysComma:    return ",";
		case DecimalWhenNEM: return (((m_decPos+1) < m_string.length()) ? "." : "");
		case CommaWhenNEM:   return (((m_decPos+1) < m_string.length()) ? "," : "");
		default:            return "";
		}
	}

	private String NonZeroSign()
	{
		String buf = "";

		switch (m_nzSign)
		{
		case AlwaysPlus:
			buf = "+";
			break;
		case PlusWhenPos:
			buf = ((m_value > 0) ? "+" : "" );
			break;
		case PlusWhenNeg:
			buf = ((m_value < 0) ? "+" : "" );
			break;
		case MinusWhenPos:
			buf = ((m_value > 0) ? "-" : "" );
			break;
		case MinusWhenNeg:
			buf = ((m_value < 0) ? "-" : "" );
			break;
		case AlwaysMinus:
			buf = "-";
			break;
		case AlwaysSign:
			buf = ((m_value > 0) ? "+" : "-" );
			break;
		case AlwaysReversed:
			buf = ((m_value > 0) ? "-" : "+" );
			break;
		default :
			// Do nothing.
			break;
		}

		return buf;
	}

	private String NonZeroAbscissa()
	{
		String buf = new String();

		int indx;
		int count = 0;
		switch (m_nzLeadZeros)
		{
		case OneLeadZero:
			if (m_decPos == 0)
				count = 1;
			break;

		case AlwaysLeadZero:
			count = m_nzAbscissa - m_decPos;
			break;
		}

		buf += Zeros( count );

		// Copy the abscissa.
		//
		for (indx = 0; indx < m_decPos; ++indx)
		{
			buf += m_string.charAt(indx);
		}

		return buf;
	}

	private String NonZeroMantissa()
	{
		if (m_nzMantissa < 1)
			return "";  // nothing to do.

		String buf = new String();

		int len = m_string.length();
		int term = m_decPos + m_nzMantissa;
		int start = m_decPos + 1;

		if (m_nzDecimal == NeverDecimal &&
			m_nzLeadZeros == NeverLeadZero)
		{
			// We are most likely dealing with a trailing zero format where
			// 0.002 should appear as X20 (assuming a mantissa of 4 digits).
			//
			// Therefore, find the first non-zero character following the decimal.

			while (true)
			{
				if (start >= term || start >= len)
					break;

				if (m_string.charAt(start) != '0')
					break;

				++start;
			}

			if (start >= term)
			{
				// The mantissa was empty, or all zeros.
				start = m_decPos + 1;
			}
		}

		// Conditionally eliminate unnecessary zeros in the mantissa,
		// working backwards from the terminal end of the string.

		if (m_nzTrailZeros == NeverTrailZero ||
			m_nzTrailZeros == OneTrailZero)
		{
			while (true)
			{
				if (term < start)
					break;

				if (term < len)
				{
					if (m_string.charAt(term) != '0')
						break;
				}

				--term;
			}
		}

		// Copy the mantissa.
		//
		if (m_nzTrailZeros == OneTrailZero && term < start)
		{
			buf += "0";
		}
		else
		{
			for (int indx = start; indx <= term; ++indx)
			{
				buf += ((indx < len) ? m_string.charAt(indx) : '0');
			}
		}

		return buf;
	}

	private double TolCalc( int nzAbscissa, int nzMantissa )
	{
		double tol = 1.0;

		if (nzMantissa > 0)
			tol = Math.pow( 0.1, (double) nzMantissa );

		// Nudge the tolerance slightly so that clients
		// such as DimAddr and MiscAddr can simply compare
		// something line (delta > Tolerance()).
		//    tol -= (tol * 1.e-3);

		return tol;
	}

	protected void finalize() throws Throwable
	{
		m_addr = null;
		m_zFormat = null;
		m_string = null;

		super.finalize();
	}

	// The data required to format non-zero numbers.
	private String m_addr;
	private int m_nzSign;
	private int m_nzLeadZeros;
	private int m_nzAbscissa;
	private int m_nzDecimal;
	private int m_nzTrailZeros;
	private int m_nzMantissa;

	// The format for a value of zero.
	private String m_zFormat;

	private double m_value;
	private double m_absValue;

	// The buffer holding the raw formatted m_absValue value.
	private String m_string;

	// The position of the decimal in m_string.
	private int m_decPos;

	private double m_tolerance;
}

