
package Weng.CodeGen;

import Weng.System.Const;
import Weng.System.Portal;
import Weng.Geometry.*;

/**
 * Use this singleton class to generate a CNC code file.
 */
public class CodeGen
{
	static public final int CM_BORE				= 0;
	static public final int CM_ROUTE			= 1;
	static public final int CM_KEEP				= 2;
	static public final int CM_TOOL				= 3;
	static public final int CM_MIXED			= 4;	// Obsolete?
	static public final int CM_MODE_UNDEFINED	= 5;

	static public final int CF_FACE_MIXED		= 0;
	static public final int CF_FACE_TOP_FIRST	= 1;
	static public final int CF_FACE_TOP_LAST	= 2;

	static public final int GS_SLICE_LL_XPOS	= 0;
	static public final int GS_SLICE_LR_XNEG	= 1;
	static public final int GS_SLICE_UR_XNEG	= 2;
	static public final int GS_SLICE_UL_XPOS	= 3;
	static public final int GS_SLICE_LL_YPOS	= 4;
	static public final int GS_SLICE_LR_YPOS	= 5;
	static public final int GS_SLICE_UR_YNEG	= 6;
	static public final int GS_SLICE_UL_YNEG	= 7;
	static public final int GS_SLICE_UNDEFINED	= 8;

	static public final int GT_TOOL_ORDER_BY_ENCOUNTER	= 0;
	static public final int GT_TOOL_ORDER_BY_JOB		= 1;
	static public final int GT_TOOL_ORDER_UNDEFINED		= 2;

	static public final int GQ_SEQ_BY_TOOL_ORDER		= 0;
	static public final int GQ_SEQ_BY_CONTAINMENT_ORDER	= 1;
	static public final int GQ_SEQ_BY_FLOW_CHART		= 2;
	static public final int GQ_SEQ_UNDEFINED			= 3;

	/**
	 * Generates a CNC code file.
	 */
	static public boolean Generate( String javaFileName, String cncFileName )
	{
		String cmd = "CodeGen:Generate:"
					 + " cgfile=\""		+ javaFileName + "\""
					 + ",ncfile=\""		+ cncFileName + "\""
					 + ",path_opt="		+ m_path_opt
					 + ",drill_opt="	+ m_drill_opt
					 + ",grid_opt="		+ m_grid_opt
					 + ",seq_opt="		+ m_seq_opt
					 + ",tool_opt="		+ m_tool_opt
					 + ",y_origin="		+ m_y_origin
					 + ",stx="			+ m_stx
					 + ",sty="			+ m_sty
					 + ",mode="			+ m_mode
					 + ",edges="		+ m_edges
					 + ",primstep="		+ m_primstep
					 + ",secnstep="		+ m_secnstep
					 + ",bidir="		+ m_bidir;

		boolean okay = Portal.Execute( cmd );

		return okay;
	}

	/**
	 * Resets the coding options to the default settings (no optimization or sequencing)
	 */
	static public void Reset()
	{
		m_drill_opt	= 0;	// (0) off / (1) on
		m_path_opt	= 0;	// (0) off / (1) on
		m_grid_opt	= 0;	// (0) off / (1) on

		m_edges		= CF_FACE_MIXED;
		m_mode		= CM_MODE_UNDEFINED;
		m_seq_opt	= GQ_SEQ_UNDEFINED;
		m_slice_opt	= GS_SLICE_UNDEFINED;
		m_tool_opt	= GT_TOOL_ORDER_UNDEFINED;
		m_bidir		= 0;	// (0) off / (1) on

		m_stx		= 0.0;
		m_sty		= 0.0;
		m_y_origin	= 0.0;
		m_primstep	= 0.0;
		m_secnstep	= 0.0;
	}

	/**
	 * Activates/Deactivates drill-drop optimization.
	 */
	static public void DrillOpt( boolean active )
	{
		m_drill_opt = (active ? 1 : 0);
	}

	/**
	 * Activates closest-point optimization.
	 * @param c_mode one of the constants starting with CM_
	 * @param c_face one of the constants starting with CF_
	 * @param startX seed point X ordinate
	 * @param startY seed point Y ordinate
	 */
	static public void ClosestOpt(
							int		c_mode,
							int		c_face,
							double	startX,
							double	startY )
	{
		m_path_opt	= 1;
		m_grid_opt	= 0;

		m_mode		= c_mode;
		m_edges		= c_face;
		m_stx		= startX;
		m_sty		= startY;

		m_tool_opt = ((c_mode == CM_TOOL) ? GT_TOOL_ORDER_BY_JOB : GT_TOOL_ORDER_BY_ENCOUNTER);
	}

	/**
	 * Activates grid optimization.
	 * @param g_slice one of the constants starting with GS_
	 * @param g_seq one of the constants starting with GQ_
	 * @param primaryStepOver cell width along the primary axis
	 * @param secondaryStepOver cell width along the secondary axis
	 * @param bidirectional grid traversal option
	 */
	static public void GridOpt(
							int		g_slice,
							double	primaryStepOver,
							double	secondaryStepOver,
							boolean	bidirectional,
							int		g_seq )
	{
		m_path_opt	= 0;
		m_grid_opt	= 1;

		m_slice_opt	= g_slice;
		m_primstep	= primaryStepOver;
		m_secnstep	= secondaryStepOver;
		m_bidir		= (bidirectional ? 1 : 0);
		m_seq_opt	= g_seq;
	}

	private CodeGen()
	{
	}

	static private int m_drill_opt	= 0;	// (0) off / (1) on
	static private int m_path_opt	= 0;	// (0) off / (1) on
	static private int m_grid_opt	= 0;	// (0) off / (1) on

	static private int m_edges		= CF_FACE_MIXED;
	static private int m_mode		= CM_MODE_UNDEFINED;
	static private int m_seq_opt	= GQ_SEQ_UNDEFINED;
	static private int m_slice_opt	= GS_SLICE_UNDEFINED;
	static private int m_tool_opt	= GT_TOOL_ORDER_UNDEFINED;
	static private int m_bidir		= 0;	// (0) off / (1) on

	static private double m_stx			= 0.0;
	static private double m_sty			= 0.0;
	static private double m_y_origin	= 0.0;
	static private double m_primstep	= 0.0;
	static private double m_secnstep	= 0.0;
}

