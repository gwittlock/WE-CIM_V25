
package Weng.CodeGen;

import Weng.System.Registry;


/**
 * Uset this singleton object to manage program numbers.
 */
public class ProgNum
{
	/**
	 * Sets the lower and upper bound on program numbers.
	 */
	static public void LimitsSet( int lowerBound, int upperBound )
	{
		Registry.IntPut( m_root, "LowerLimit", lowerBound );
		Registry.IntPut( m_root, "UpperLimit", upperBound );
	}

	/**
	 * Gets the 'next' program number and increments the program number by 1.
	 */
	static public int NextGet()
	{
		CondLimitsInit();
		CondNextInit();

		int lowerBound = Registry.IntGet( m_root, "LowerLimit" );
		int upperBound = Registry.IntGet( m_root, "UpperLimit" );

		int next = Registry.IntGet( m_root, "Next" );
		if (next < lowerBound || next > upperBound)
			next = lowerBound;

		NextSet( next + 1 );

		return next;
	}

	static private void NextSet( int nextNum )
	{
		Registry.IntPut( m_root, "Next", nextNum );
	}

	static private void CondLimitsInit()
	{
		String tmp;
		
		tmp = Registry.StringGet( m_root, "LowerLimit" );
		if (tmp == null || tmp.length() < 1)
			Registry.IntPut( m_root, "LowerLimit", 9999 );
		
		tmp = Registry.StringGet( m_root, "UpperLimit" );
		if (tmp == null || tmp.length() < 1)
			Registry.IntPut( m_root, "UpperLimit", 9999 );
	}

	static private void CondNextInit()
	{
		String tmp = Registry.StringGet( m_root, "Next" );
		if (tmp == null || tmp.length() < 1)
			Registry.IntPut( m_root, "Next", 1 );
	}

	private ProgNum()
	{
	}

	static private String m_root = Registry.REGROOT + "\\Customizations\\ProgNum";
}

