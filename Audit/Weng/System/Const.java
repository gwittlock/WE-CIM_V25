
package Weng.System;

/**
 * Building Blocks Inc. system constant definitions.
 */
public class Const
{
	// Use standard java class Color
	// static public final int WHITE	= 0xffffff;
	// static public final int RED		= 0x0000ff;
	// static public final int GREEN	= 0x00ff00;
	// static public final int BLUE	= 0xff0000;
	// static public final int CYAN	= 0xffff00;
	// static public final int YELLOW	= 0x00ffff;
	// static public final int MAGENTA	= 0xff00ff;

	/**
	 * Generally used to indicate uninitialized integer and double values.
	 */
	static public final double UNDEFINED   = 1.e9;

	/**
	 * Used as system measure for coincidence (between points, for instance).
	 */
	static public final double SMALL       = 1.e-6;

	/**
	 * Used as system measure for vector products and parameter-space.
	 */
	static public final double VECTOR_SMALL = 1.e-12;

	/**
	 * Radian equavalent of 180 degrees.
	 */
	static public final double PI          = Math.PI;

	/**
	 * Radian equavalent of 360 degrees.
	 */
	static public final double TWOPI       = (2.0 * Math.PI);

	/**
	 * Radian equavalent of 270 degrees.
	 */
	static public final double THREEHALFPI = (1.5 * Math.PI);

	/**
	 * Radian equavalent of 90 degrees.
	 */
	static public final double HALFPI      = (0.5 * Math.PI);

	/**
	 * Radian equavalent of 45 degrees.
	 */
	static public final double QUARTERPI   = (0.25 * Math.PI);

	/**
	 * Radian equavalent of 720 degrees.
	 */
	static public final double FOURPI      = (4.0 * Math.PI);
	
	/**
	 * Conversion factor between radians and degrees.
	 */
	static public final double RAD2DEG     = (180.0 / Math.PI);

	/**
	 * Conversion factor between degrees and radians.
	 */
	static public final double DEG2RAD     = (Math.PI / 180.0);

	/**
	 * Clockwise direction.
	 */
	static public final int CW  = -1;

	/**
	 * Counter-clockwise direction.
	 */
	static public final int CCW =  1;

	/**
	 * Offset to the left.
	 */
	static public final int LEFT  =  1;

	/**
	 * Offset to the right.
	 */
	static public final int RIGHT = -1;

	// OBSOLETE
	// Layer entity type.
	//
	// static public final int LAYER		= 0;

	/**
	 * Workplane entity type.
	 */
	static public final int WORKPLANE	= 1;

	/**
	 * Tool entity type.
	 */
	static public final int TOOL		= 2;

	/**
	 * Point entity type.
	 */
	static public final int POINT		= 3;

	/**
	 * Line entity type.
	 */
	static public final int LINE		= 4;

	/**
	 * Arc entity type.
	 */
	static public final int ARC			= 5;

	/**
	 * Hole entity type.
	 */
	static public final int HOLE		= 6;

	/**
	 * Profile entity type.
	 */
	static public final int PROFILE		= 7;

	/**
	 * Command entity type.
	 */
	static public final int COMMAND		= 8;

	/**
	 * Feature entity type.
	 */
	static public final int FEATURE		= 9;

	/**
	 * Pattern entity type.
	 */
	static public final int PATTERN		= 10;

	/**
	 * Sequence entity type.
	 */
	static public final int SEQUENCE	= 11;


	static public final int ROUTER_BIT		= 1;
	static public final int DISC_SAW		= 2;
	static public final int BRAD_POINT		= 3;
	static public final int LANCE_BIT		= 4;
	static public final int COUNTER_SINK	= 5;
	static public final int AGGREGATE		= 6;
	static public final int TAP				= 7;
	static public final int DRILL			= 8;
	static public final int BURNER			= 9;
	static public final int SCRIBE			= 10;
	static public final int POWDER_MARK		= 11;
	static public final int CENTER_PUNCH	= 12;
	static public final int LASER			= 13;
	static public final int WATERJET		= 14;
	static public final int ROUND			= 15;
	static public final int SQUARE			= 16;
	static public final int RECTANGLE		= 17;
	static public final int OBROUND			= 18;
	static public final int DIAMOND			= 19;
	static public final int CORNER_RADIUS	= 20;
	static public final int SINGLE_D		= 21;
	static public final int DOUBLE_D		= 22;
	static public final int TRAPEZOID		= 23;
	static public final int KEYHOLE			= 24;
	static public final int FORMING			= 25;
	static public final int MARKING			= 26;
	static public final int HEXAGON			= 27;
	static public final int END_MILL		= 28;
	static public final int CUSTOM			= 29;
	
	private Const()
	{
	}
}
