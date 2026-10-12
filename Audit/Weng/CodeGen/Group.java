
package Weng.CodeGen;

import Weng.System.Const;

/**
 * Use a Group object to manage the relationship between Symbol objects.
 * <br>
 * For instance, the group of symbols representing motion codes could simply be
 * <br>
 * <pre>
 *     MOTION = {G00,G01,G02,G03}
 * </pre>
 * A more complex relationship might be
 * <br>
 * <pre>
 *     MOTION = {LINEAR_MOTION,ARC_MOTION}
 *     LINEAR_MOTION = {G00,G01}
 *     ARC_MOTION = {G02,G03}
 * </pre>
 * <br>
 * These latter relationships can be graphically illustrated by:
 * <br>
 * <img SRC="Groups.jpg" height=300 width=300>
 * <p>
 * Rules to be noted:
 * <br>
 * A Group can be a subset of only one Group.
 * <br>
 * A Group can have many subsets.
 * <br>
 * A Group can have many symbols.
 * <p>
 * See also CodeGen.doc
 */
public class Group
{
	/**
	 * Creates a Group object as a subset of the given parent group
	 * @param parent (null) when this object represents the root group
	 *               (otherwise) an existing group object
	 */
	public Group( Group parent )
	{
		m_parent  = parent;

		m_groups  = new Group[10];
		m_groupCount  = 0;

		m_symbols = new Symbol[10];
		m_symbolCount = 0;

		m_currSymbol  = null;

		if (m_parent != null)
			m_parent.Add( this );
	}

	/**
	 * Generates a Symbol string corresponding to the given value if and
	 * only if the given value is different from this objects value.  This
	 * objects value is updated accordingly.
	 */
	public String Cond( int value )
	{
		Symbol symbol = Find( value );

		if (symbol == null)
			return "";  // really an assertion

		return symbol.Cond();
	}

	/**
	 * Generates a Symbol string corresponding to the given value regardless
	 * of this objects value.  This objects value is updated accordingly.
	 */
	public String Uncond( int value )
	{
		Symbol symbol = Find( value );

		if (symbol == null)
			return "";  // really an assertion

		return symbol.Uncond();
	}

	public boolean Update( Symbol symbol )
	{
		boolean change;

		if (m_parent != null)
		{
			change = m_parent.Update( symbol );
		}
		else
		{
			change = (symbol != m_currSymbol);
			m_currSymbol = symbol;
		}

		return change;
	}

	/**
	 * Gets this objects current Symbol value
	 */
	public Symbol Symbol()
	{
		return m_currSymbol;
	}

	public int Value()
	{
		if (m_currSymbol == null)
			return (int) Const.UNDEFINED;
		else
			return m_currSymbol.Value();
	}

	public int Add( Symbol symbol )
	{
		Symbol result = Find( symbol.Value() );

		if (result != null)
			return -1;  // already in table.

		m_symbols[ m_symbolCount ] = symbol;
		++m_symbolCount;

		return 0;
	}

	private int Add( Group group )
	{
		for (int indx = 0; indx < m_groupCount; ++indx)
		{
			if (m_groups[indx] == group)
				return -1;  // already in group.
		}

		m_groups[ m_groupCount ] = group;
		++m_groupCount;

		return 0;
	}

	// Check this node, then the leaves.
	private Symbol Find( int value )
	{
		for (int indx = 0; indx < m_symbolCount; ++indx)
		{
			if (m_symbols[indx] == null)
				break;

			if (m_symbols[indx].Value() == value)
				return m_symbols[indx];
		}

		for (int jndx = 0; jndx < m_groupCount; ++jndx)
		{
			Symbol result = m_groups[jndx].Find( value );

			if (result != null)
				return result;
		}

		return null;
	}

	protected void finalize() throws Throwable
	{
		m_groups = null;
		m_symbols = null;

		super.finalize();
	}

	private Group m_parent;
	private Group [] m_groups;
	private Symbol [] m_symbols;
	private int m_groupCount;
	private int m_symbolCount;
	private Symbol m_currSymbol;
}

