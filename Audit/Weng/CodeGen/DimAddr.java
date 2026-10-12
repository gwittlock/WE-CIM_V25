
package Weng.CodeGen;

import Weng.System.*;


/**
 * An object of the class DimAddr is used to represent a machine
 * register that tracks dimensional information, such as tool
 * position.  In addition to tracking state, a DimAddr object
 * can generate formatted strings representing the current state,
 * in a form that is accepted by the target machine tool.
 *
 * A change in position is recorded if the delta between the
 * current register value and a candidate value exceeds the
 * dimensional tolerance of the register, as derived from its
 * word format.
 */
public class DimAddr
{
	/**
	 * Construct a DimAddr object having fixed output format.
	 * @param theAddr the register address symbol ('X' for instance)
	 * @param nzSign  the signed-value convention
	 * @param nzLeadZeros the lead-zero convention
	 * @param nzAbscissa  the number of digits preceeding the implied decimal position
	 * @param nzDecimal   the decimal point convention
	 * @param nzTrailZeros the trailing-zero convention
	 * @param nzMantissa  the number of digits following the implied decimal position
	 * @param zFormat  the appearance of the value of 'zero'
	 */
	public DimAddr( String theAddr,
				 int    nzSign,
				 int    nzLeadZeros,
				 int    nzAbscissa,
				 int    nzDecimal,
				 int    nzTrailZeros,
				 int    nzMantissa,
				 String zFormat )
	{
		m_addrFmt = new AddrFmt( theAddr, nzSign, nzLeadZeros,
								 nzAbscissa, nzDecimal, nzTrailZeros,
								 nzMantissa, zFormat );

		m_tolerance = m_addrFmt.Tolerance();

		// Slightly nudge the tolerance for use by Delta()
		m_tolerance -= (m_tolerance * 1.e-3);

		m_curr = -999999.0;
		m_shift = 0.0;
		
		m_min = -Const.UNDEFINED;
		m_max = Const.UNDEFINED;
		
		m_track = false;
	}

	/**
	 * When track is false, this register tracks the world ordinate as
	 * seen by the model (default behavior).  When track is true, this
	 * register tracks the world ordinate as seen by the machine register.
	 * This latter case is intended for incremental programs where you
	 * want ot prevent cumulative positioning errors.
	 */
	public void TrackRoundedValues( boolean track )
	{
		m_track = track;
	}
	
	/**
	 * Get the current register value.
	 */
	public double Curr()
	{
		return m_curr;
	}

	/**
	 * Determines whether the difference between the given value
	 * and the current value of this register exceeds the dimensional
	 * tolerance of this register.  Note, a delta considers any shift
	 * value, ie. delta = | curr - (value + shift) |
	 * @param value the value to compare against this register
	 * @return (true) the difference between the given value and this
	 *                registers value exceeds this registers dimesional
	 *                tolerance.
	 */
	public boolean Delta( double value )
	{
		double delta = Math.abs(m_curr - (value + m_shift));

		return (delta > m_tolerance);
	}

	/**
	 * Generates a formatted string representing this registers
	 * absolute (as opposed to incremental) value and updates the
	 * current value to the given value, if and only if the
	 * difference between the given value and the current value
	 * of this register exceeds the dimensional tolerance of this
	 * register.
	 * @return ("") when there is no change / (else) a formatted string
	 *         representing the given value
	 */
	public String AbsCond( double value )
	{
		m_buffer = "";

		value += m_shift;
		
		if ( Delta(value) )
		{
			m_buffer = m_addrFmt.Format( value );
			m_curr = ((m_track == true) ? m_addrFmt.RoundedValue() : value);

			IntegrityCheck();
		}
		
		return m_buffer;
	}

	/**
	 * Generates a formatted string representing this registers
	 * absolute (as opposed to incremental) value and updates the
	 * current value to the given value, regardless of the
	 * difference between the given value and the current value
	 * of this register.
	 * @return a formatted string representing the given value
	 */
	public String AbsUncond( double value )
	{
		value += m_shift;
		
		m_buffer = m_addrFmt.Format( value );
		m_curr = ((m_track == true) ? m_addrFmt.RoundedValue() : value);

		IntegrityCheck();

		return m_buffer;
	}

	/**
	 * Generates a formatted string representing the difference
	 * between this registers current value and the given, and
	 * updates the current value to the given value, if and only
	 * if the difference between the given value and the current
	 * value of this register exceeds the dimensional tolerance
	 * of this register.
	 * @return ("") when there is no change / (else) a formatted string
	 *         representing the difference between this registers
	 *         current value and the given value
	 */
	public String IncrCond( double value )
	{
		m_buffer = "";

		value += m_shift;
		
		if ( Delta(value) )
		{
			double delta = value - m_curr;
			m_buffer = m_addrFmt.Format( delta );
			m_curr = ((m_track == true) ? m_addrFmt.RoundedValue( value ) : value);

			IntegrityCheck();
		}

		return m_buffer;
	}

	/**
	 * Generates a formatted string representing the difference
	 * between this registers current value and the given, and
	 * updates the current value to the given value, regardless
	 * of the difference between the given value and the current
	 * value of this register.
	 * @return a formatted string representing the difference
	 *         between this registers current value and the
	 *         given value
	 */
	public String IncrUncond( double value )
	{
		double delta;

		value += m_shift;
		delta = value - m_curr;
		m_buffer = m_addrFmt.Format( delta );
		m_curr = ((m_track == true) ? m_addrFmt.RoundedValue( value ) : value);

		IntegrityCheck();

		return m_buffer;
	}

	/**
	 * Sets the current value of this register.  Often required
	 * at the start of post processing to emulate the machines
	 * starting position (especially critical for machines that
	 * run in incremental mode).
	 */
	public void Set( double value )
	{
		m_curr = value;
	}

	/**
	 * Gets the shift value associated with this register.
	 */
	public double Shift()
	{
		return m_shift;
	}

	/**
	 * Sets the shift value associated with this register, essentially
	 * translating by the shift amount, any value output via the methods
	 * AbsCond(), AbsUncond(), IncrCond() and IncrUncond().
	 */
	public void Shift( double shift )
	{
		m_shift = shift;
	}

	/**
	 * Sets this registers' limits.  By using this method, anytime output
	 * is generated with an 'out of range' value, an error message is issued
	 * and an exception is raised.  On this latter issue, if the main loop 
	 * of the code generator contains a try/catch clause, the exception will
	 * provide information for a stack trace, providing clues as to the
	 * origin of the problem.
	 */
	public void Limits( double min, double max )
	{
		m_min = min;
		m_max = max;
	}

	private void IntegrityCheck()
	{
		if (m_curr < m_min || m_curr > m_max)
		{
			Msg.Display( "Value out of range [" + m_min + ".." + m_max + "]\n"
						+ "  word <" + m_buffer + ">  value <" + m_curr + ">" );

			// Force an exception.  If a try/catch clause exists in the
			// main loop of the code generator, the exception will allow
			// a stack trace to be printed, providing clues as to the
			// origin of the limit violation.
			int bar = 0;
			int foo = 1 / bar;
		}
	}
		
	protected void finalize() throws Throwable
	{
		m_addrFmt = null;

		super.finalize();
	}

	AddrFmt m_addrFmt;
	double  m_tolerance;
	String  m_buffer;
	double  m_curr;
	double	m_shift;
	double	m_min;
	double	m_max;
	boolean	m_track;
}

