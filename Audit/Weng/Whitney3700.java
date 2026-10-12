// cg.Prog|str|Program Number|O
// cg.switch|str|G61/G64|G61
// cg.units|combo|Units|Inch|Metric|Inch to Metric|Metric to Inch
// cg.pierce_delay|combo|Pierce Delay|Off|On
// cg.slide_movelarge|combo|Large Part Slide|Off|On
// cg.slide_doorlarge|combo|  Open Door|None|Before Slide|After Slide
// cg.slide_distlarge|dbl|  Slide Distance|0
// cg.slide_movesmall|combo|Small Part Slide|Off|On
// cg.slide_distsmall|dbl|  Slide Distance|0
// cg.stdpunch|combo|Standard Punch|Normal|Extended|Tool Change
// cg.forming|combo|Forming/Marking|Normal|Extended|Tool Change
// cg.load|combo|Material Loading|M87(MANUAL)|M88(AUTOMATIC)|M170(PLATEPARTNER)
// cg.xload|dbl|Load X|0.0
// cg.yload|dbl|Load Y|-65.0
// cg.Feed1|dbl|Feed rate|100
// cg.Rapid|dbl|Rapid rate|600

import Weng.System.*;
import Weng.Modeler.*;
import Weng.Math.*;
import Weng.Geometry.*;
import Weng.CodeGen.*;
import java.io.*;
import java.util.*;
import Weng.Access.*;

// 3/12/2003 -- Updated by CG_V15_TO_V16
public class Whitney3700 implements WengMacro
{
	// When required, this user-defined variable declaration
	// should be moved to the top of this code generator file.
	//    cg.coordinate_mode|combo|Movement|Absolute|Incremental
	
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Frequently modified variable & constants.

	private final boolean DEBUG = false;
	private final boolean REC = false;
	private final boolean TYPE = false;

	private double RAPID_RATE = 600.;		// ipm (previously a constant)
	private final double DROP_DOOR_TIME = 7. / 60.;
	private final double TOOL_CHANGE_TIME = 10. / 60.;
	private final double PUNCH_HIT_TIME = 0.4 / 60.;
	private final double BURN_START_TIME = 0.5 / 60.;

	private boolean m_useSeqNum = true;
	private int m_seqNum = 1;
	private int m_seqNumInc = 2;

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// The main processing loop.
	//
	public void main()
	{
		Initialize();
		
		int fileStatus = output.Open();
		boolean earlyTerminate;

		try
		{
			ToolReport();

			while ( !clfile.AtEnd() )
			{
				clfile.Read();

				// For debugging purposes ....
				if (DEBUG && REC) output.Dump( "  >> record type: " + clfile.RecType() );

				switch ( clfile.RecType() )
				{
				case Clfile.eClampInfo:
					ClampInfo();
					break;
				case Clfile.eStartProgEnd:
					StartProgram();
					break;
				case Clfile.eEndProgEnd:
					EndProgram();
					break;
				case Clfile.eRapid:
					Disengage();
					break;
				case Clfile.eLine:
					Line();
					break;
				case Clfile.eCwArc:
				case Clfile.eCcwArc:
					Arc();
					break;
				case Clfile.eDrillHole:
					Hole();
					break;
				case Clfile.eToolChangeEnd:
					ToolChange();
					break;
				case Clfile.eHoldCommand:
//					CondOutput( G54.Uncond() + G00.Uncond() + G70.Uncond() + X.AbsUncond( clfile.Xe() ) + L.Uncond( clfile.Dbl("width")-clfile.Ye()));
//					break;
				
				
					// 8/6/2003 -- Gary, trickery to avoid bogus hold & repo.
					clfile.StatePush();
					
					clfile.Read();
					if (DEBUG && TYPE) output.Dump( "  >> record type: " + clfile.RecType() );
					earlyTerminate = (clfile.RecType() == Clfile.eEndProgEnd);
					
					if ( !earlyTerminate )
					{
						clfile.Read();
						if (DEBUG && TYPE) output.Dump( "  >> record type: " + clfile.RecType() );
						earlyTerminate = (clfile.RecType() == Clfile.eEndProgEnd); //64 ?
					}
					
					if ( !earlyTerminate )
					{
						clfile.Read();
						if (DEBUG && TYPE) output.Dump( "  >> record type: " + clfile.RecType() );
						earlyTerminate = (clfile.RecType() == Clfile.eEndProgEnd);  //65?
					}
					
					if ( earlyTerminate )
					{
						EndProgram();
					}
					else
					{
						clfile.StatePop();
						
						CondOutput(
							G54.Uncond()
							+ G00.Uncond()
							+ X.AbsUncond( clfile.Xe() )
							+ L.Uncond( Model.DoubleGet("width")-clfile.Ye() ) );
					}
					break;

				case Clfile.eRepoCommand:
					Repo();
					break;
				case Clfile.eDropCommand:
					Drop();
					break;
				case Clfile.eStopCommand:
					Stop();
					break;
				}
			}

			output.Close();
			Finalize();
		}
		catch(Exception e)
		{
			ExceptionPrinter.StackTracePrint(e);
		}
	}

	private void Initialize()
	{
		clfile = new Clfile();
		output = new OutSys();

		ClassVarsInit();
		SymbolsInit();
		G40.Uncond();
		M41.Uncond();
		
		G175X.Set(0.);
		
		ClampsInit();
	}

	private void Finalize()
	{
		clfile = null;
		output = null;
	}
  
	private void ToolReport()
	{
		int [] toolNos = new int[50];
		int indx, count = 0;
		
		clfile.Read();
		output.Dump ("%");

		output.Dump( m_Prog );
		output.Dump( "(FILE -- " + CNCFile() + ")" );
		output.Dump( "(------ BEGIN TOOL REPORT ------)" );
		
		while ( !clfile.AtEnd() )
		{
			clfile.Read();
			if (clfile.RecType() == Clfile.eToolChangeEnd)
			{
			    int toolNo = clfile.Int("NC_Code_Number");

				if ( IsValidToolNo( toolNo ) )
				{
					for (indx = 0; indx < count; ++indx)
					{
						if (toolNo == toolNos[indx])
							break;
					}
					
					if (indx >= count)
					{
						toolNos[count] = toolNo;
						++count;

						output.Dump( "(TOOL -- " + T.Uncond( toolNo ) + " " + ToolDesc() + ")" );
					}
				}
				else
				{
					Msg.Display( "Error in ToolReport(): Invalid tool number -- " + toolNo );
					// Intentionally cause an exception to stop processing.
					toolNo /= 0;
				}
			}
		}
		
		output.Dump( "(------ END TOOL REPORT ------)" );
		
		clfile.Rewind();
	}

	private void StartProgram()
	{
		Calendar calendar = Calendar.getInstance();
		Date date = calendar.getTime();
		String time = date.toString();
		int day   = calendar.get(java.util.Calendar.DATE);
		int month = calendar.get(java.util.Calendar.MONTH) + 1;
		int year  = calendar.get(java.util.Calendar.YEAR);

		int nextToolNo = NextToolNo();

		if (DEBUG) output.Dump( "====> StartProgram()" );
		//output.Dump( "%" );
		//output.Dump( "(FILE -- " + CNCFile() + ")" );
		output.Dump( m_Prog );
		output.Dump( "(00:00PM -- " + month + "/" + day + "/" + year + ")" );
		output.Dump( "(CODED FOR 3700ATC-RR TRUECUT)" );
		output.Dump( "(CG VER. DATE 6/8/2000)" );
		output.Dump( "(MATERIAL: " + clfile.Str( "MatCfg" ) + ")" );
		output.Dump( "(LENGTH: " + clfile.Str( "Length" ) + ")" );
		output.Dump( "(WIDTH: " + clfile.Str( "Width" ) + ")" );
		output.Dump( "(THICKNESS: " + clfile.Str( "Thickness" ) + ")" );

		CondOutput( UNITS.Uncond( (m_metric ? 1 : 0) ) );
		CondOutput( ABSINC.Uncond( (m_incr ? 1 : 0) ) );
		CondOutput( G40.Uncond() );
		CondOutput( G54.Uncond() );
		CondOutput( "M31" );
		CondOutput( PIERCE.Uncond( m_pierce ) );
		CondOutput( "M46" );
	    CondOutput( m_switch );

		if (nextToolNo != UNDEFINED)
			CondOutput( T.Uncond( nextToolNo ) );

		CondOutput( "G64(PUNCHING MODE)" );  // TODO "G61 (NIBBLING MODE)"
		CondOutput( "#512=" + F.Uncond( Feedrate() ) + "(SET FEEDRATE)" );  // TODO
	  	CondOutput( "M85" );

		TableVelocity();

		if ( HaveClamps() )
		{
			CondOutput( "G178"
						+ X.AbsUncond( m_clamp[0].X() )
						+ Y.AbsUncond( m_clamp[1].X() )
						+ "(CLAMP SETTINGS)" );
		}
		
		CondOutput( X.AbsUncond( m_xLoad ) + Y.AbsUncond( m_yLoad ) + m_load); //LOAD.Uncond( m_load ) );
	}

	private String Rapid( double xe, double ye, boolean force )
	{
		String block = "";

		m_engaged = false;

		if ( X.Delta( xe ) || Y.Delta( ye ) )
		{
			double dx = xe - X.Curr();
			double dy = ye - Y.Curr();

			m_time += (Math.sqrt( dx*dx + dy*dy ) / RAPID_RATE);

			if ( m_incr )
				block = G00.Uncond() + X.IncrUncond( xe ) + Y.IncrUncond( ye );
			else
				block = G00.Uncond() + X.AbsUncond( xe ) + Y.AbsUncond( ye );
		}

		return block;
	}

	private void Line()
	{
		double xe = clfile.Xe() * m_cf;
		double ye = clfile.Ye() * m_cf;

		Line line = clfile.Line( );
		m_time += (line.ArcLen( ) / Feedrate());

		Engage();

		if ( X.Delta( xe ) || Y.Delta( ye ) )
		{
			String block;
			
			if (DEBUG) output.Dump( "====> Line()" );

			if ( m_incr )
				block = G01.Cond() + X.IncrUncond( xe ) + Y.IncrUncond( ye );
			else
				block = G01.Cond() + X.AbsUncond( xe ) + Y.AbsUncond( ye );

			if (COMP.Symbol() != G40 && clfile.Entity().IsLeadOut())
				block += (G40.Uncond() + D.Uncond(0));

			CondOutput( block );
		}
	}

	private void Arc()
	{
		double xs = clfile.Xs() * m_cf;
		double ys = clfile.Ys() * m_cf;
		double xe = clfile.Xe() * m_cf;
		double ye = clfile.Ye() * m_cf;
		double xc = clfile.Xc() * m_cf;
		double yc = clfile.Yc() * m_cf;

		Arc arc = clfile.Arc();
        m_time += (arc.ArcLen( ) / Feedrate());

		Engage();
			
		if (DEBUG) output.Dump( "====> Arc()" );

		if ( m_incr )
		{
			CondOutput( ARC.Cond( clfile.RecType() ) +
						X.IncrUncond( xe ) + Y.IncrUncond(ye ) +
						I.Uncond( xc - xs ) + J.Uncond( yc - ys ) );
		}
		else
		{
			CondOutput( ARC.Cond( clfile.RecType() ) +
						X.AbsUncond( xe ) + Y.AbsUncond(ye ) +
						I.Uncond( xc - xs ) + J.Uncond( yc - ys ) );
		}
	}

	private void Hole()
	{
		double xe = clfile.Xe() * m_cf;
		double ye = clfile.Ye() * m_cf;

		if ( X.Delta( xe ) || Y.Delta( ye ) )
		{
			String block;
			double dx, dy;

			Point ps = new Point( X.Curr(), Y.Curr(), 0. );
			Point pe = new Point( xe, ye, 0. );

			boolean avoided = ConditionalClampsAvoid( ps, pe );

			if (DEBUG) output.Dump( "====> Hole()" );

			dx = pe.X() - ps.X();
			dy = pe.Y() - ps.Y();
			m_time += (Math.sqrt( dx*dx + dy*dy ) / RAPID_RATE);

			if ( IsPunchTool() )
			{
				CondOutput( M75.Cond() );

				block = X.AbsUncond( xe ) + Y.AbsUncond( ye );
				if ( AutoIndex() )
					block += C.Cond( IndexAngle() );

				CondOutput( block );

				++m_hits;

				m_time += PUNCH_HIT_TIME;

				ConditionalSlugsDump();
			}
			else
			{
				// drill/tap/scribe ?  // TODO
				//CondOutput( " -- tap" );
				//m_time += 0.6; //.25 = 15 seconds
			}
		}
	}

	private void EndProgram()
	{
		String unloadPosition = ((m_metric)
							 ? X.AbsUncond(3048.0) + Y.AbsUncond(-1612.9)
							 : X.AbsUncond(120.0) + Y.AbsUncond(-63.5));
		
		Disengage();
							 
		if (DEBUG) output.Dump( "====> EndProgram()" );
		
		CondOutput( G54.Uncond() );
		CondOutput( G40.Uncond() );
		CondOutput( "M31" );
		CondOutput( G00.Uncond() + G70.Uncond() + unloadPosition + "(EDIT FOR SKELETON REMOVAL)" );
		CondOutput( "M97" );
		CondOutput( "(CYCLE TIME:" + CT.Uncond( m_time ) + " MIN.)" );
		CondOutput( " " );
		CondOutput( "M30" );
		output.Dump( "%" );
	}

	private void ToolChange()
	{
		int nextToolNo = NextToolNo();
		
		if (DEBUG) output.Dump( "====> ToolChange()" );
		
		CondOutput( M42.Cond() );

		CondOutput( "G106" );
		CondOutput( M85.Cond() );

		m_currToolNo = ToolNo();
		CondOutput( T.Uncond( m_currToolNo ) + "M06 (" + ToolDesc() + ")" );
		// CondOutput( "( future tool comment )" );

		if (nextToolNo != UNDEFINED)
			CondOutput( T.Cond( nextToolNo ) );  
			
		CondOutput( OffsetCode() );   
		
		CondOutput( PressureCode( m_currToolNo ) );

		if ( IsPunchTool() )
			Stripper();

		m_time += TOOL_CHANGE_TIME;
	}

	private void Engage()
	{
		if ( !m_engaged )
		{
			String block;
	
			double xs = clfile.Xs() * m_cf;
			double ys = clfile.Ys() * m_cf;
	
			Point ps = new Point( X.Curr(), Y.Curr(), 0. );
			Point pe = new Point( xs, ys, 0. );
	
			boolean avoided = ConditionalClampsAvoid( ps, pe );
			
			if (DEBUG) output.Dump( "====> Engage()" );
			
			if ( IsPunchTool() )
			{
				block = Rapid( xs, ys, false );
	
				if ( AutoIndex() )
					block += C.Cond( IndexAngle() );
	
				CondOutput( block );
			}
			else
			{
				double dx, dy;
				
				double xe = clfile.Xe() * m_cf;
				double ye = clfile.Ye() * m_cf;
	
				// Assume some type of burner.
				int cutside = clfile.Int( "cutside" );
	
				CondOutput( OffsetCode() + G00.Uncond() + X.AbsUncond( xs ) + Y.AbsUncond( ys ));//Rapid( xs, ys, false ));
				CondOutput( "F#512" + M17.Uncond() + "G61");
				m_time += BURN_START_TIME;
				
				dx = xe - xs;
				dy = ye - ys;
				m_time += (Math.sqrt( dx*dx + dy*dy ) / Feedrate());
	
				block = G01.Cond() + X.AbsUncond( xe ) + Y.AbsUncond( ye );
	
				if (cutside != 0)
				{
					int doff = clfile.Int( "doff" );
					block += COMP.Uncond( cutside ) + D.Uncond( 1 );
				}
	
				CondOutput( block );
			}
	
			m_engaged = true;
		}
	}
	
	private void Disengage()
	{
		if ( m_engaged )
		{
			if (DEBUG) output.Dump( "====>Disengage()" );

			CondOutput( M18.Cond() );  // turn off torch
			
			m_engaged = false;
		}
	}

	private boolean IsValidToolNo( int toolNo )
	{
		toolNo %= 100;
		
		return (toolNo > 0 && toolNo < 16);
	}

	private boolean IsPunchTool()
	{
		int type = clfile.Int( "Type_ID" );
		return IsPunchTool( type );
	}
	
	private boolean IsPunchTool( int type )
	{
		return (type >= Const.ROUND && type <= Const.HEXAGON);
	}
	
	private boolean AutoIndex()
	{
		boolean autoIndex = ((clfile.Int( "auto_index" ) == 1) ? true : false);

		if ( autoIndex )
		{
			int type = clfile.Int( "Type_ID" );
			autoIndex = (type != Const.ROUND);
		}

		return autoIndex;
	}
	
	private double IndexAngle()
	{
		return clfile.Dbl( "orient" );
	}

	private void Stripper()
	{
		String sval;
		int type = clfile.Int( "Type_ID" );
		
		if (type == Const.FORMING)
		{
			sval = Model.StringGet("cg.forming");

			if ( sval.equalsIgnoreCase("Normal") )
				CondOutput( "G177M55(NORMAL STRIPPER HEIGHT)" );				
			else if ( sval.equalsIgnoreCase("Extended") )
				CondOutput( "G177M54(EXTENDED STRIPPER HEIGHT)" );				
			else
				CondOutput( "G177M56(TOOL CHANGE STRIPPER HEIGHT)" );				
		}
		else
		{
			sval = Model.StringGet("cg.stdpunch");

			if ( sval.equalsIgnoreCase("Tool Change") )
				CondOutput( "G177M56(TOOL CHANGE STRIPPER HEIGHT)" );				
			else if ( sval.equalsIgnoreCase("Extended") )
				CondOutput( "G177M54(EXTENDED STRIPPER HEIGHT)" );				
			else
				CondOutput( "G177M55(NORMAL STRIPPER HEIGHT)" );				
		}
	}

	private void ConditionalSlugsDump()
	{
		if (m_hits < m_maxHits)
			return;
			
		if (DEBUG) output.Dump( "====> ConditionalSlugsDump()" );

		CondOutput( "M20(SLUG DUMP AFTER " + m_hits + " HOLES)" );

		m_hits = 0;
	}

	private String OffsetCode()
	{
		String gcode = "";

		int type = clfile.Int( "Type_ID" );

		if ( IsPunchTool( type ) )
//			gcode = G54.Uncond() + "(PUNCHING SEQUENCES FOLLOW)";
			gcode = G54.Uncond() ;
		else if (type == Const.BURNER)
//			gcode = G55.Uncond() + "(PLASMA SEQUENCES FOLLOW)";
			gcode = G55.Uncond() ;
		else if (type == Const.TAP)
			gcode = G56.Uncond() + "(TAPPING SEQUENCES FOLLOW)";
		else if (type == Const.SCRIBE ||
				 type == Const.POWDER_MARK ||
				 type == Const.MARKING)
			gcode = G57.Uncond() + "(MARKING SEQUENCES FOLLOW)";

		return gcode;
	}

	private String PressureCode( int toolNo )
	{
		String mcode = "";

		int type = clfile.Int( "Type_ID" );

		if (type == Const.FORMING)
		{
			mcode = M41.Cond();
		}
		else if (type == Const.SCRIBE ||
			 	 type == Const.POWDER_MARK ||
			 	 type == Const.MARKING)
		{
			mcode = M40.Cond();
		}

		return mcode;
	}

	private int ToolNo()
	{
		return ( clfile.Int( "NC_Code_Number" ) );
	}

	private int NextToolNo()
	{
		int nextToolNo = UNDEFINED;

		clfile.StatePush();

		while (true)
		{
			if ( clfile.AtEnd() )
				break;

			clfile.Read();

			if (clfile.RecType() == Clfile.eToolChangeEnd)
			{
				nextToolNo = clfile.Int("NC_Code_Number");
				break;
			}
		}

		clfile.StatePop();

		return nextToolNo;
	}

	private String ToolDesc()
	{
		String toolDesc = "";
		String sval = clfile.Str( "Description" ).toUpperCase();
		if (sval.length() > 0)
			toolDesc = sval;
		return toolDesc;
	}

	private String CNCFile()
	{
		String CNCFileName = "VIEWER";
		String CNCFile = output.Path();
		if (CNCFile != null)
		{
			int PeriodPos = CNCFile.lastIndexOf('.');
			int SlashPos = CNCFile.lastIndexOf('\\') + 1;
			if (SlashPos >= 0 && PeriodPos > SlashPos)
				CNCFileName = CNCFile.substring(SlashPos,PeriodPos);
		} 	 
		return CNCFileName.toUpperCase();
	}

	private String SeqNum()
	{
		String buf = "";

		if ( m_useSeqNum )
		{
			buf = N.Uncond( m_seqNum ) + "";
			m_seqNum += m_seqNumInc;
		}

		return buf;
	}

	private void CondOutput( String block )
	{
		if (block.length() > 0)
			output.Dump( SeqNum() + block );
	}

	private void Repo()
	{
		// NOTE: In this case, clfile.Xe() is an incremental distance
		// as extracted from the @REPO attribute 'x' parameter.

		if (DEBUG) output.Dump( "====> Repo()" );

		int repo = clfile.Int( "repo" );
		double repoLocation = clfile.Xe();
		double delta = repoLocation - G175X.Curr();
		
//		CondOutput("(REPOSITION TO FOLLOW)");
		CondOutput( G175X.IncrUncond( repoLocation ) + "Y-1.27" );

		m_time += (delta / RAPID_RATE);

		ClampUpdate( m_clamp[0], delta ); 
		ClampUpdate( m_clamp[1], delta );  	

		if (DEBUG)
		{
			output.Dump( "clamp1 xmin:" + m_clamp[0].Xmin() + " ymin:" + m_clamp[0].Ymin()
						+ " xmax:" + m_clamp[0].Xmax() + " ymax:" + m_clamp[0].Ymax() );
			output.Dump( "clamp2 xmin:" + m_clamp[1].Xmin() + " ymin:" + m_clamp[1].Ymin()
						+ " xmax:" + m_clamp[1].Xmax() + " ymax:" + m_clamp[1].Ymax() );
		}
	}
	
	private void Drop()
	{
		if (DEBUG) output.Dump( "====> Drop()" );

		String slide = Model.StringGet( "cg.slide_movesmall" ).trim();
		double dist = Model.DoubleGet( "cg.slide_distsmall" );
		
		CondOutput( M19.Cond() );
		
		m_time += DROP_DOOR_TIME;
		
		if ( slide.equalsIgnoreCase("On") && Math.abs( dist ) > 1.e-3 )
		{
			CondOutput("(SLIDE MOVE TO FOLLOW)");
			output.Dump( SeqNum() + G01.Uncond() + X.AbsCond( X.Curr() + dist) + "F400." );
			
			m_time += (dist / 400.);
		}                         

		CondOutput( "M80" );
		CondOutput( "M01" );
	}
	
	private void Stop()
	{
		if (DEBUG) output.Dump( "====> Stop()" );

		String slide = Model.StringGet( "cg.slide_movelarge" ).trim();
		String door = Model.StringGet( "cg.slide_doorlarge" ).trim();
		double dist = Model.DoubleGet( "cg.slide_distlarge" );
		
		CondOutput( M19.Cond() );

		if ( door.equalsIgnoreCase("Before Slide") )
			CondOutput( "M80" );
		
		if ( slide.equalsIgnoreCase("On") && Math.abs( dist ) > 1.e-3 )
		{
			CondOutput("(SLIDE MOVE TO FOLLOW)");
			output.Dump( SeqNum() + G01.Uncond() + X.AbsCond( X.Curr() + dist ) + "F400." );				
			
			m_time += (dist / 400.);
		}

		if ( door.equalsIgnoreCase("After Slide") )
			CondOutput( "M80" );
		
		CondOutput( "M00" );
	}

	private boolean HaveClamps()
	{
		return (m_clamp[0] != null && m_clamp[1] != null);
	}

	private void ClampsInit()
	{
		m_clamp = new SafeZone[2];
		m_clamp[0] = null;
		m_clamp[1] = null;
	}


	private void ClampInfo()
	{
		DbEntity	zone;
		String		attribName;
		double		lldx, lldy;
		double		trdx, trdy;
		double		xclamp, yclamp;
		double		width;
		int			count, indx;
		
		zone = clfile.Entity();
		count = zone.IntGet("_clamp_num");
		if (count < 1)
			return;
		
		if (DEBUG) output.Dump("CLAMP INFO");
		
		width = Model.DoubleGet("Width");
		lldx = zone.DoubleGet("_clamp_lldx");
		lldy = zone.DoubleGet("_clamp_lldy");
		trdx = zone.DoubleGet("_clamp_trdx");
		trdy = zone.DoubleGet("_clamp_trdy");
		
		for (indx = 0; indx < count; ++indx)
		{
			m_clamp[indx] = new SafeZone();
			
			attribName = "_clamp" + (indx+1) + "_x";
			xclamp = zone.DoubleGet( attribName );
			
			attribName = "_clamp" + (indx+1) + "_y";
			yclamp = zone.DoubleGet( attribName );
			
			m_clamp[indx].Set(
				xclamp, yclamp,
				(xclamp + lldx), (yclamp + lldy - width),
				(xclamp + trdx), (yclamp + trdy - width) );
		}
	}

	private boolean ConditionalClampsAvoid( Point ps, Point pe )
	{
		boolean avoided = false;

		if ( !HaveClamps() )
			return avoided;  // early exit (not a nested sheet)

		if ( m_clamp[0].Collision( ps, pe, 0. ) ||
			 m_clamp[1].Collision( ps, pe, 0. ) )
		{
			String comment = " (AVOIDING CLAMP)";
			
			if (DEBUG) output.Dump( "====> ConditionalClampsAvoid()" );

			if (ps.Y() > m_clamp[0].Ymin())
			{
				CondOutput( G70.Uncond() + Rapid( ps.X(), m_clamp[0].Ymin(), false ) + comment );
				comment = "";
				avoided = true;
			}

			if (pe.Y() > m_clamp[0].Ymin())
			{
				CondOutput( G70.Uncond() + Rapid( pe.X(), m_clamp[0].Ymin(), false ) + comment );
				avoided = true;
			}
		} 

		return avoided;
	}

	private void ClampUpdate( SafeZone clamp, double repoAmount )
	{
		if ( HaveClamps() )
		{
			double xmin = clamp.Xmin() + repoAmount;
			double ymin = clamp.Ymin();
			double xmax = clamp.Xmax() + repoAmount;
			double ymax = clamp.Ymax();
	
			// NOTE: SafeZone.m_x & SafeZone.m_y are not really used.
			clamp.Set( 0., 0., xmin, ymin, xmax, ymax );
		}
	}
                                                         
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Initialization methods
	//

	private void ClassVarsInit()
	{
		// cg.units|combo|Units|Metric to Inch|Inch to Metric|Metric|Inch
		String sval = Model.StringGet("cg.units");
		
		if ( sval.equalsIgnoreCase("Inch") )
		{
			m_metric = false;
			m_cf = 1.0;
		}
		else if ( sval.equalsIgnoreCase("Inch to Metric") )
		{
			m_metric = true;
			m_cf = 25.4;
		}
		else if ( sval.equalsIgnoreCase("Metric") )
		{
			m_metric = true;
			m_cf = 1.0;
		}
		else if ( sval.equalsIgnoreCase("Metric to Inch") )
		{
			m_metric = false;
			m_cf = 1.0 / 25.4;
		}

		// cg.coordinate_mode|combo|Movement|Absolute|Incremental
		//    sval = clfile.Str( "cg.coordinate_mode" );
		//    m_incr = ((sval.compareToIgnoreCase("Incremental") == 0) ? 1 : 0);
		m_incr = false;

		// cg.pierce_delay|combo|Pierce Delay|Off|On
		sval = Model.StringGet( "cg.pierce_delay" );
		m_pierce = ((sval.compareToIgnoreCase("On") == 0) ? 1 : 0);

		// cg.slide_movesmall|combo|Small Part Slide|Off|On
		sval = Model.StringGet( "cg.slide_movesmall" ).trim();
		m_slidemovesmall = ((sval.compareToIgnoreCase("On") == 0) ? 1 : 0);

		// cg.slide_movelarge|combo|Large Part Slide|Off|On
		sval = Model.StringGet( "cg.slide_movelarge" ).trim();
		m_slidemovelarge = ((sval.compareToIgnoreCase("On") == 0) ? 1 : 0);

		// cg.load|combo|Material Loading|Manual|Automatic
		m_load = Model.StringGet( "cg.load" );
		m_Prog = Model.StringGet( "cg.Prog" );
		m_switch = Model.StringGet( "cg.switch" );         

		// cg.xload|double|Load X|0.0
		// cg.yload|double|Load Y|0.0
		m_xLoad = Model.DoubleGet( "cg.xload" );
		m_yLoad = Model.DoubleGet( "cg.yload" );

		m_clamp = new SafeZone[2];
		m_clamp[0] = null;
		m_clamp[1] = null;
		
		RAPID_RATE = Model.DoubleGet( "cg.Rapid" );
		m_feed  = Model.DoubleGet( "cg.Feed1" );
		
		m_engaged = false;
	}
	
	private void TableVelocity()
	{
		// 0.454 pounds / kilogram
		double factor = ((m_metric) ? 0.454 : 1.0);
		
		double materialWeight = clfile.Dbl("materialweight") * factor;  // TODO: get material weight from where?

		if (materialWeight > 0 && materialWeight < (330 * factor))
			m_tableVelocity = 1;
		else if (materialWeight >= (330 * factor) && materialWeight < (550 * factor))
			m_tableVelocity = 2;
		else if (materialWeight >= (550 * factor) && materialWeight < (1100 * factor))
			m_tableVelocity = 3;
		else if (materialWeight >= (1000 * factor))
			m_tableVelocity = 4;
		else
			m_tableVelocity = 0;
			
		if (DEBUG) output.Dump( "====> TableVelocity()" );

		if (m_tableVelocity >=1 && m_tableVelocity <= 5) 
			CondOutput( "G150M6" + m_tableVelocity );
		else if (m_tableVelocity == 0)
			CondOutput( "M00(RAMP RATES CANNOT BE DETERMINED !)" );
		else
			CondOutput( "M00(MACHINE WEIGHT LIMITATION EXCEEDED !)" );
	}

	private double Feedrate()
	{
		return m_feed;	// clfile.Int( "feed" );
	}

	private void SymbolsInit()
	{
		// For reference:
		//     AddrFmt( addr, nzSign, nzLeadZeros, nzAbscissa, nzDecimal,
		//                nzTrailZeros, nzMantissa, zFormat) 

        if ( m_metric )
		{
			X = new DimAddr(  "X", 5, 1, 4, 1, 1, 3, "0.0" );
			Y = new DimAddr(  "Y", 5, 1, 4, 1, 1, 3, "0.0" );
			I = new MiscAddr( "I", 5, 1, 4, 1, 0, 3, "0.0" );
			J = new MiscAddr( "J", 5, 1, 4, 1, 0, 3, "0.0" );
			G175X = new DimAddr(  "G175X", 6, 1, 4, 1, 1, 3, "0" );
			L = new MiscAddr(  "Y", 6, 1, 4, 1, 1, 3, "0" );
		}
		else
		{
			X = new DimAddr(  "X", 5, 1, 3, 1, 1, 4, "0.0" );
			Y = new DimAddr(  "Y", 5, 1, 3, 1, 1, 4, "0.0" );
			I = new MiscAddr( "I", 5, 1, 3, 1, 0, 4, "0.0" );
			J = new MiscAddr( "J", 5, 1, 3, 1, 0, 4, "0.0" );
			G175X = new DimAddr(  "G175X", 6, 1, 3, 1, 1, 4, "0" );
			L = new MiscAddr(  "Y", 6, 1, 4, 1, 1, 3, "0" );
		}

		N = new MiscAddr( "N", 0, 0, 4, 0, 0, 0, "0" );
		C = new MiscAddr( "C", 0, 0, 3, 1, 0, 2, "0.0" );
		T = new MiscAddr( "T", 0, 0, 3, 0, 0, 0, "0" );
		D = new MiscAddr( "D", 0, 0, 2, 0, 0, 0, "00" );
		F = new MiscAddr( "", 0, 0, 5, 1, 0, 0, "0" );
		CT = new MiscAddr( " ", 5, 0, 5, 1, 0, 2, "0" );


		UNITS  = new Group( null );
		ABSINC = new Group( null );
		COMP   = new Group( null );
		TOR    = new Group( null );

		PRESS  = new Group( null );
		OFFSET = new Group( null );
		STOP   = new Group( null );
		PUNCH  = new Group( null );
		LOAD   = new Group( null );
		PIERCE = new Group( null );
		MOTION = new Group( null );
		LINEAR = new Group( MOTION );
		ARC    = new Group( MOTION );

		G00 = new Symbol( LINEAR, "G00", Clfile.eRapid );
		G01 = new Symbol( LINEAR, "G01", Clfile.eLine );
		G02 = new Symbol( ARC,    "G02", Clfile.eCwArc );
		G03 = new Symbol( ARC,    "G03", Clfile.eCcwArc );

		G54 = new Symbol( OFFSET, "G54",  0 );	// activate punch offset
		G55 = new Symbol( OFFSET, "G55",  1 );	// activate torch offset
		G56 = new Symbol( OFFSET, "G56",  2 );	// activate tap offset
		G57 = new Symbol( OFFSET, "G57",  3 );	// activate marking offset

		G20 = new Symbol( UNITS,  "G20",  0 );	// inch
		G21 = new Symbol( UNITS,  "G21",  1 );	// metric

		G40 = new Symbol( COMP,   "G40",  0 );	// off
		G41 = new Symbol( COMP,   "G41",  1 );	// left
		G42 = new Symbol( COMP,   "G42", -1 );	// right

		M17 = new Symbol( TOR,   "M17",  0 );	// TORCH ON
		M18 = new Symbol( TOR,   "M18",  1 );	// TORCH OFF DROP DOOR CLOSED
		M19 = new Symbol( TOR,   "M19", -1 );	// TORCH OFF DROP DOOR OPEN

		G90 = new Symbol( ABSINC, "G90",  0 );	// absolute
		G91 = new Symbol( ABSINC, "G91",  1 );	// incremental

		M00 = new Symbol( STOP,   "M00",  0 );	// hard stop
		M01 = new Symbol( STOP,   "M01",  1 );	// optional stop

		M40 = new Symbol( PRESS,  "M40",  0 );	// marking 

		M41 = new Symbol( PRESS,  "M41 (PRESSURE MODE ON)",  1 );	// forming pressure
		M42 = new Symbol( PRESS,  "M42 (PRESSURE MODE OFF)",  2 );	// cancel

		M85 = new Symbol( PRESS,  "M85",  0 );	// disable punching ram (disengage clutch)
		M75 = new Symbol( PRESS,  "M75",  1 );	// enable punching ram
		G70 = new Symbol( PRESS,  "G70",  2 );  // inhibit punch

		M87 = new Symbol( LOAD,   "M87(MANUAL LOAD)", 0 );
		M88 = new Symbol( LOAD,   "M88(AUTO LOAD)",   1 );
		M170 = new Symbol( LOAD,  "M170(AUTO LOAD)",  2 );  

		M15 = new Symbol( PIERCE, "M15(PIERCE DELAY OFF)", 0 );
		M65 = new Symbol( PIERCE, "M65(PIERCE DELAY ON)",  1 );
	}

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Declarations
	//

	Clfile clfile = null;
	OutSys output = null;

	int UNDEFINED = (int) Const.UNDEFINED;
	int m_currToolNo = 0;
	int m_drillToolNo = 0;
	double m_time = 0;

	boolean	m_incr		= false;
	boolean	m_metric	= false;
	double m_cf			= 1.;		// Units conversion factor

	String	m_load			= null;    
	boolean	m_engaged		= false;
	double	m_xLoad			= 0.0;
	double	m_yLoad			= 0.0;
	String	m_Prog			= null;
	String	m_switch		= null;
	double	m_feed			= 100.;

	int		m_tableVelocity	= 0;
	int		m_pierce		= 0;
	int		m_slidemovesmall	= 0;
	int		m_slidemovelarge	= 0;             

	int		m_maxHits	= 100;
	int		m_hits		= 0;

	DimAddr  X = null;
	DimAddr  Y = null;
	MiscAddr I = null;
	MiscAddr J = null;
	MiscAddr C = null;
	MiscAddr N = null;
	MiscAddr T = null;
	MiscAddr D = null;
	MiscAddr F = null;
	MiscAddr CT = null;
	DimAddr G175X = null;
	MiscAddr L = null;


	Group UNITS  = null;
	Group ABSINC = null;
	Group TOR    = null;
	Group COMP   = null;
	Group PRESS  = null;
	Group OFFSET = null;
	Group STOP   = null;
	Group PUNCH  = null;
	Group LOAD   = null;
	Group PIERCE = null;
	Group MOTION = null;
	Group LINEAR = null;
	Group ARC    = null;

	Symbol G00 = null;
	Symbol G01 = null;
	Symbol G02 = null;
	Symbol G03 = null;

	Symbol G54 = null;
	Symbol G55 = null;
	Symbol G56 = null;
	Symbol G57 = null;

	Symbol G20 = null;
	Symbol G21 = null;

	Symbol G40 = null;
	Symbol G41 = null;
	Symbol G42 = null;

	Symbol M17 = null;
	Symbol M18 = null;
	Symbol M19 = null;

	Symbol G90 = null;
	Symbol G91 = null;

	Symbol M00 = null;
	Symbol M01 = null;

	Symbol M40 = null;
	Symbol M41 = null;
	Symbol M42 = null;

	Symbol M75 = null;
	Symbol M85 = null;
	Symbol G70 = null;

	Symbol M87 = null;
	Symbol M88 = null;
	Symbol M170 = null;  

	Symbol M15 = null;
	Symbol M65 = null;

	SafeZone [] m_clamp = null;
}



