
package Weng.CodeGen;

/**
 * Use an object of class Symbol to represent miscellaneous
 * and preparatory function codes, such as G01 and M00.  As
 * function codes often override each other, Symbol objects
 * should be related to each other via a Group object.
 */
public class Symbol
{
	/**
	 * Creates a symbol object that is related to the given group.
	 * @param group the parent group of this symbol (<b>can</b> be null)
	 * @param the appearance of this symbol in CNC code
	 * @param this symbols numeric value (usually represents a clfile event)
	 */
	public Symbol( Group group, String theSymbol, int value )
	{
		m_group = group;

		m_symbol = theSymbol;
		m_value = value;

		if (m_group != null)
			m_group.Add( this );
	}

	/**
	 * Generates this symbols string representation if and only if
	 * this symbols value overrides its parents current value, and
	 * sets its parents value to this symbols value.
	 */
	public String Cond()
	{
		if (m_group == null)
			return m_symbol;

		boolean changed = m_group.Update( this );

		return ((changed == true) ? m_symbol : "");
	}

	/**
	 * Generates this symbols string representation regardless of
	 * this symbols parents current value, and sets its parents
	 * value to this symbols value.
	 */
	public String Uncond()
	{
		if (m_group == null)
			return m_symbol;

		m_group.Update( this );

		return m_symbol;
	}

	public int Value()
	{
		return m_value;
	}

	private Group m_group;
	private String m_symbol;
	private int m_value;
}

