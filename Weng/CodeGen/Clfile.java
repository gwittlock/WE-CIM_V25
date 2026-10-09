
package Weng.CodeGen;

import Weng.System.Const;
import Weng.System.Msg;
import Weng.System.Portal;
import Weng.Geometry.*;
import Weng.Modeler.DbEntity;


/**
 * A Clfile object provides access to the cutter-location and
 * and supporting data required to generate CNC code for a
 * machine tool.  Each time a clfile object reads a database
 * record, the record data is transfered to an internal data
 * buffer.  As the data is retrieved in a piecewise fashion (that
 * is, each record may contain different types of information)
 * the internal buffer represents the accumulated state of all
 * clfile data processed to the current point in time.
 *
 * Each record in the clfile database is associated with an
 * event type.  You can use these event types to trigger various
 * code generator functionality.
 *
 * Convience methods are provided for accessing ordinate and
 * geometric information.  This class is also augmented by methods
 * that allow you to obtain named-attribute information that may
 * be either system or user defined.
 */
public class Clfile
{
	public Clfile( )
	{
		Rewind();
	}

	/**
	 * Determines whether you are at the end of the clfile.
	 * @return true when at the end of the clfile.
	 */
	public boolean AtEnd( )
	{
		return (RecType() < 0);
	}

	/**
	 * Resets this objects current database record to be the
	 * first record in the clfile database.
	 */
	public void Rewind( )
	{
		m_recNo = -1;
		m_recType = 0;
		m_entityID = 0;
	}

	/**
	 * Gets the next clfile record.
	 */
	public int Read( )
	{
		// m_recType is tracked as an integer array because Java
		// can only pass arrays of intrinsic types to C++.  The
		// other option is to use the Integer class.
		//
		int tmpRecInfo[] = new int[2];

        m_recNo = Read( m_recNo+1, tmpRecInfo, m_int, m_dbl );

        m_recType = ((m_recNo < 0) ? -1 : tmpRecInfo[0]);
		m_entityID = tmpRecInfo[1];

        return m_recNo;
	}

	/**
	 * Gets the event type of the current clfile record.
	 */
	public int RecType()          { return m_recType; }

	/**
	 * Gets the record number of the current clfile record.
	 */
	public int RecNo()            { return m_recNo; }

	/**
	 * Gets a proxy to the entity associated with the current record.
	 * @return <b>null</b> when not associated.
	 */
	public DbEntity Entity()
	{
		if (m_entityID == 0)
			return null;

		m_proxy.Id( m_entityID );

		return m_proxy;
	}

	/**
	 * Reads the specified clfile record and makes it the current record.
	 * This method is typically used in conjuction with the method StatePush()
	 * when you need to perform 'look-ahead' operations.
	 */
	public int Seek( int recNo )
	{
		m_recNo = recNo - 1;
		return Read();
	}

	/**
	 * Saves the current state of the clfile so that it can be restored
	 * later using the method StatePop().
	 */
	public void StatePush()
	{
		int recInfo[] = new int[3];
		recInfo[0] = m_recNo;
		recInfo[1] = m_recType;
		recInfo[2] = m_entityID;
		StatePush( recInfo, m_int, m_dbl );
	}

	/**
	 * Restores a previously saved clfile state.
	 */
	public void StatePop()
	{
		// The record number and type are defaulted in case of failure.
		int recInfo[] = new int[3];
		recInfo[0] = m_recNo;
		recInfo[1] = m_recType;
		recInfo[2] = m_entityID;
		StatePop( recInfo, m_int, m_dbl );
		m_recNo = recInfo[0];
		m_recType = recInfo[1];
		m_entityID = recInfo[2];
	}

	/**
	 * Advances the clfile forward to the next entity, updating
	 * the accumulated state of the clfile.
	 */
	public int NextEntity()
	{
		int recType;

		while (true)
		{
			Read();

			recType = RecType();

			if (recType < 0)
				return -1;  // Reached end of clfile.

			if (recType >= eRapid && recType <= ePeckHole)
				return recType;  // Found next entity
		}
	}

	///////////////    double value access methods   /////////////////////
	/////////////// Must be careful to match indices /////////////////////
	///////////////    with there counterparts in    /////////////////////
	///////////////          ClfileTypes.h           /////////////////////

	/**
	 * Gets the X ordinate of the start point of the current entity.
	 */
	public double Xs()	{ return m_dbl[0]; }

	/**
	 * Gets the Y ordinate of the start point of the current entity.
	 */
	public double Ys()	{ return m_dbl[1]; }

	/**
	 * Gets the Z ordinate of the start point of the current entity.
	 */
	public double Zs()	{ return m_dbl[2]; }

	/**
	 * Gets the X ordinate of the end point of the current entity.
	 */
	public double Xe()	{ return m_dbl[3]; }

	/**
	 * Gets the Y ordinate of the end point of the current entity.
	 */
	public double Ye()	{ return m_dbl[4]; }

	/**
	 * Gets the Z ordinate of the end point of the current entity.
	 */
	public double Ze()	{ return m_dbl[5]; }

	/**
	 * Gets the X ordinate of the center point of the current arc.
	 */
	public double Xc()	{ return m_dbl[6]; }

	/**
	 * Gets the Y ordinate of the center point of the current arc.
	 */
	public double Yc()	{ return m_dbl[7]; }

	/**
	 * Gets the Z ordinate of the center point of the current arc.
	 */
	public double Zc()	{ return m_dbl[8]; }

	/**
	 * Gets the depth value of the current hole.
	 */
	public double Depth()	{ return m_dbl[9]; }

	/**
	 * Gets the count of records in the clfile.
	 */
	public native int Count();

	/**
	 * Gets the value of a named-attribute as an integer.
	 * @return (Const.UNDEFINED) when the attributed does not exist
	 */
	public native int Int( String attributeName );

	/**
	 * Gets the value of a named-attribute as a double.
	 * @return (Const.UNDEFINED) when the attributed does not exist
	 */
	public native double Dbl( String attributeName );

	/**
	 * Gets the value of a named-attribute as a string.
	 * @return (null) when the attributed does not exist
	 */
	public native String Str( String attributeName );

	/**
	 * Create a geometric line from the current clfile record.
	 * NOTE: this only makes sense when the current record
	 * represents a line.
	 */
	public Line Line()
	{
		Line line = new Line(
						m_dbl[0], m_dbl[1], m_dbl[2],
						m_dbl[3], m_dbl[4], m_dbl[5] );
		return line;
	}

	/**
	 * Create a geometric arc from the current clfile record.
	 * NOTE: this only makes sense when the current record
	 * represents a arc.
	 */
	public Arc Arc()
	{
		int dir = ((m_recType == Clfile.eCcwArc) ? 1 : -1);

		Arc arc = new Arc(
						m_dbl[0], m_dbl[1], m_dbl[2],
						m_dbl[3], m_dbl[4], m_dbl[5],
						m_dbl[6], m_dbl[7], m_dbl[8],
						dir );
		return arc;
	}

	/////////////// Values returned by method RecType() /////////////////////
	///////////////   Must be careful to match these    /////////////////////
	///////////////     with there counterparts in      /////////////////////
	///////////////            ClfileTypes.h            /////////////////////

	static public final int eRapid              = 0;
	static public final int eLine               = 1;
	static public final int eCwArc              = 2;
	static public final int eCcwArc             = 3;

	static public final int eFirstPoint         = 20;
	static public final int eFirstDrillHole     = 21;
	static public final int eFirstPeckHole      = 22;

	static public final int ePoint              = 23;
	static public final int eDrillHole          = 24;
	static public final int ePeckHole           = 25;

	static public final int eDisengage			= 43;
	static public final int eTextCommand        = 44;
	static public final int eUserCommand        = 45;
	static public final int eHoldCommand        = 46;
	static public final int eRepoCommand        = 47;
	static public final int eStopCommand        = 48;
	static public final int eDropCommand        = 49;
	static public final int eClampInfo          = 50;
	static public final int eSpeedRampInfo      = 51;

	static public final int eMainBegin          = 60;
	static public final int eMainEnd            = 61;
	static public final int eStartProgBegin     = 62;
	static public final int eStartProgEnd       = 63;
	static public final int eEndProgBegin       = 64;
	static public final int eEndProgEnd         = 65;
	static public final int eToolChangeBegin    = 66;
	static public final int eToolChangeEnd      = 67;

	static public final int eSubDefBegin        = 80;
	static public final int eSubDefEnd          = 81;
	static public final int eSubCall            = 82;


	////////////////// private methods ///////////////////////

	// Directly accesses m_int[] and m_dbl[]
	private native int Read( int recNo, int[] recType, int[] foo, double[] bar );
	private native void StatePush( int[] recInfo, int[] intData, double[] dblData );
	private native void StatePop( int[] recInfo, int[] intData, double[] dblData );

	static
	{
		String libName = "mm2CodeGen";

		try
		{
			System.loadLibrary( libName );
		}
		catch (UnsatisfiedLinkError e)
		{
			Msg.Display( "Fatal Error: class Clfile failed to load library "+libName );
		}
	}

	protected void finalize() throws Throwable
	{
		m_int = null;
		m_dbl = null;

		super.finalize();
	}

	////////////////// private data ///////////////////////

	// The current clfile record index.
	private int m_recNo;

	// The current clfile record type.
	private int m_recType;

	private int m_entityID;

	// The buffers for standard values.
	private int[]    m_int = new int[16];
	private double[] m_dbl = new double[16];

	DbEntity m_proxy = new DbEntity();
}

