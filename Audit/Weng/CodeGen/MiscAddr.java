
package Weng.CodeGen;

/**
 * An object of the class MiscAddr is used to represent a machine
 * register that tracks non-dimensional information, such as feedrates
 * and spindle speeds.  In addition to tracking state, a MiscAddr object
 * can generate formatted strings representing the current state,
 * in a form that is accepted by the target machine tool.
 *
 * A change in state is recorded if the delta between the
 * current register value and a candidate value exceeds the
 * dimensional tolerance of the register, as derived from its
 * word format.
 */
public class MiscAddr
{
	/**
	 * Construct a MiscAddr object having fixed output format.
	 * @param theAddr the register address symbol ('X' for instance)
	 * @param nzSign  the signed-value convention
	 * @param nzLeadZeros the lead-zero convention
	 * @param nzAbscissa  the number of digits preceeding the implied decimal position
	 * @param nzDecimal   the decimal point convention
	 * @param nzTrailZeros the trailing-zero convention
	 * @param nzMantissa  the number of digits following the implied decimal position
	 * @param zFormat  the appearance of the value of 'zero'
	 */
	public MiscAddr( String theAddr,
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
	 * tolerance of this register.
	 * @param value the value to compare against this register
	 * @return (true) the difference between the given value and this
	 *         registers value exceeds this registers dimesional tolerance.
	 */
	public boolean Delta( double value )
	{
		double delta = Math.abs(m_curr - value);

		return (delta > m_tolerance);
	}

	/**
	 * Generates a formatted string representing this registers
	 * value and updates the current value to the given value,
	 * if and only if the difference between the given value
	 * and the current value of this register exceeds the
	 * dimensional tolerance of this register.
	 * @return ("") when there is no change / (else) a formatted string
	 *         representing the given value
	 */
	public String Cond( double value )
	{
		m_buffer = "";

		if ( Delta(value) )
		{
			m_buffer = m_addrFmt.Format( value );
			m_curr = value;
		}

		return m_buffer;
	}

	/**
	 * Generates a formatted string representing this registers
	 * value and updates the current value to the given value,
	 * regardless of the difference between the given value and
	 * the current value of this register.
	 * @return a formatted string representing the given value
	 */
	public String Uncond( double value )
	{
		m_buffer = m_addrFmt.Format( value );
		m_curr = value;

		return m_buffer;
	}

	/**
	 * Sets the current value of this register.  Often required
	 * at the start of post processing to emulate the machines
	 * starting condition.
	 */
	public void Set( double value )
	{
		m_curr = value;
	}

	protected void finalize() throws Throwable
	{
		m_addrFmt = null;
		m_buffer = null;

		super.finalize();
	}

	AddrFmt m_addrFmt;
	double  m_tolerance;
	String  m_buffer;
	double  m_curr;
}

