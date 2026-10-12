// cg.prog|int|Program #|3333
// cg.units|combo|Units|Inch|Metric|Inch to Metric|Metric to Inch
// cg.Feed1|dbl|Feed|225
// cg.repo_load|dbl|Repo Load Y|-10.187
// cg.pierce_delay|combo|Pierce Delay|On|Off
// cg.slide_movelarge|combo|Large Part Slide|Off|On
// cg.slide_movesmall|combo|Small Part Slide|Off|On
// cg.stripper|combo|Stripper|Normal|Extended|Other
// cg.load|combo|Material Loading|M87(MANUAL)|M88(AUTOMATIC)|M170(PLATEPARTNER)                                     
// cg.xload|dbl|Load X|0.0
// cg.yload|dbl|Load Y|-35.0


import Weng.System.*;
import Weng.Modeler.*;
import Weng.Math.*;
import Weng.Geometry.*;
import Weng.CodeGen.*;
import Weng.Access.*;
import java.io.*;
import java.util.*;

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Kuest -- ATC661
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// 2007.01.25 (PE) -- Initial implementation.
// 2007.03.07 (PE) -- Clamp avoidance to Y-7.
// 2007.03.21 (PE) -- Add LAA (G76)
// 2007.04.16 (PE) -- Canned Cycles
// 2007.04.22 (PE) -- Clamps avoidance when nibbling entities.
// 2007.05.12 (PE) -- Added display of clamp avoidance movements.
//
public class Whitney implements WengMacro
{
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Frequently modified variable & constants.

	private final boolean DEBUG = true;

	private boolean m_useSeqNum = true;
	private int m_seqNum = 1;
	private int m_seqNumInc = 2;

	private final double DRILL_DELTA = 0.375;
	
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// The main processing loop.
	//
	public void main()
	{
		Initialize();

		int fileStatus = output.Open();

		try
		{
			//ToolReport();

			while ( !clfile.AtEnd() )
			{
				clfile.Read();

				// For debugging purposes ....
				// output.Dump( "  >> record type: " + clfile.RecType() );

				switch ( clfile.RecType() )
				{
				case Clfile.eClampInfo:
					ClampInfo();
					break;
				case Clfile.eStartProgEnd:
					StartProgram();
					break;
				case Clfile.eEndProgEnd:
					Disengage();
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
					Hold();
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
				//case Clfile.eDisengage:
				//	Disengage();
				//	break;
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
		M42.Uncond();
		M25.Uncond();
		
		X.Set(0.);
		Y.Set(0.);
		Z.Set(0.);
		
		G175X.Set(0.);
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
		ProgNum.LimitsSet( 1000, 9000 );
		
		//output.Dump(" ");
		output.Dump( "%" );
		// output.Dump(":" + ProgNum.NextGet());
		output.Dump( O.Uncond( Model.IntGet("cg.prog") ) );
		output.Dump( "(------ BEGIN TOOL REPORT ------)" );

		while ( !clfile.AtEnd() )
		{
			clfile.Read();
			if (clfile.RecType() == Clfile.eToolChangeEnd)
			{
			    int toolNo = clfile.Int("NC_Code_Number");

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

		
		output.Dump( "(FILE -- " + CNCFile() + ");");
		// output.Dump( "(CODE REVISION: "  + time.toUpperCase()  + ")" );		
		// output.Dump( "(00:00PM -- " + month + "/" + day + "/" + year + ")" );
		// output.Dump( "(CODED FOR 661ATC-RR TRUECUT)" );
		// output.Dump( "(CG VER. DATE 6/8/2000)" );
		// output.Dump( "(MATERIAL: " + clfile.Str( "MatCfg" ) + ")" );
		// output.Dump( "(LENGTH: " + clfile.Str( "Length" ) + ")" );
		// output.Dump( "(WIDTH: " + clfile.Str( "Width" ) + ")" );
		// output.Dump( "(THICKNESS: " + clfile.Str( "Thickness" ) + ")" );
														 
		//CondOutput( UNITS.Uncond( (m_metric ? 1 : 0) ) );
		//CondOutput( ABSINC.Uncond( (m_incr ? 1 : 0) ) );
		//CondOutput( "M98 P1" );
		CondOutput( G54.Uncond() );

		CondOutput( "G00M85" );
		CondOutput( "G106" );
//		CondOutput( "#521=" + F.Cond( m_Feed ) + "(SET FEEDRATE)" );  // TODO

		TableVelocity();

		if ( HaveClamps() )
		{
			CondOutput( "G178"
						+ X.AbsUncond( m_clamp[0].X() )
						+ Y.AbsUncond( m_clamp[1].X() )
						+ "(CLAMP SETTINGS)" );
		}

		CondOutput( X.AbsUncond( m_xLoad ) + Y.AbsUncond( m_yLoad ) + m_load );
	}

	private String Rapid( double xe, double ye, boolean force )
	{
		String block = "";

		m_engaged = false;

		if ( X.Delta( xe ) || Y.Delta( ye ) )
		{
			if (DEBUG) output.Dump( "====> Rapid()" );

			if ( m_incr )
				block = G00.Uncond() + X.IncrUncond( xe ) + Y.IncrUncond( ye );
			else
				block = G00.Uncond() + X.AbsUncond( xe ) + Y.AbsUncond( ye );
		}

		return block;
	}

	private void Line()
	{
		double xs = clfile.Xs() * m_cf;
		double ys = clfile.Ys() * m_cf;
		double xe = clfile.Xe() * m_cf;
		double ye = clfile.Ye() * m_cf;

		Line line = clfile.Line(); 
		m_time += (line.ArcLen() / Feedrate()); 

		if ( X.Delta( xe ) || Y.Delta( ye ) )
		{
			String block;

			if (DEBUG) output.Dump( "====> Line()" );
			
			if ( IsPunchTool() )
			{
				double lineAng = line.StartTan().Radians() * Const.RAD2DEG;
				double punchLen = PunchLength();
				double lineLen = line.ArcLen() + punchLen;
				int nHits = (int) (lineLen / punchLen) + 1;
				
				double punchDist = lineLen / nHits;
				
				if (DEBUG)
				{
					output.Dump( "line length:" + lineLen );
					output.Dump( "punch length" + punchLen );
					output.Dump( "# of hits:" + nHits );
				}
				
				Engage();
				CondOutput( X.AbsCond( xs ) + Y.AbsCond( ys ) );
				
				// Use the Line Cycle.
				CondOutput( G76.Uncond()
							+ I.Uncond( punchDist )
							+ J.Uncond( lineAng )
							+ K.Uncond( nHits ) );
	
				// Set the XY registers to bogus values so that both will
				// be output if the next entity generates separate hits.
				X.Set( xe + 1.e-3 );
				Y.Set( ye + 1.e-3 );
			}
			else
			{
				Engage();
				
				if ( m_incr )
					block = G01.Cond() + X.IncrUncond( xe ) + Y.IncrUncond( ye );
				else
					block = G01.Cond() + X.AbsUncond( xe ) + Y.AbsUncond( ye );
	
				CondOutput( block );
			}
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
			
			int toolType = clfile.Int("Type_ID");

			Point ps = new Point( X.Curr(), Y.Curr(), 0. );
			Point pe = new Point( xe, ye, 0. );

			boolean avoided = ConditionalClampsAvoid( ps, pe );

			if (DEBUG) output.Dump( "====> Hole()" );
			
			if ( IsPunchTool( toolType ) )
			{
				DbEntity dbEntity = clfile.Entity();
				int sncs = dbEntity.IntGet("sncs");
				
				CondOutput( M75.Cond() );

				block = X.AbsUncond( xe ) + Y.AbsUncond( ye );
				if ( AutoIndex() )
					block += C.Cond( IndexAngle() );

				CondOutput( block );

				if ((sncs >= 302) && (sncs <= 305))
				{
					// canned cycle.
					double	dst = dbEntity.DoubleGet("dst");
					double	ang = dbEntity.DoubleGet("ang");
					int		cnt = dbEntity.IntGet("cnt");
					
					CondOutput( G76.Uncond() + I.Uncond(dst) + J.Uncond(ang) + K.Uncond(cnt) );

					m_hits = m_hits + (cnt - 1);
				}
				
				++m_hits;
				m_time += 0.012; //.25 = 15 seconds
				ConditionalSlugsDump();
			}
			else if ( IsDrillTap( toolType ) )
			{
				// N67G100A5.5B697.0Z0.875M24(DRILL CYCLE)
				// N69M66(WARPED COMP OFF)
				double depth = clfile.Depth() + DRILL_DELTA;
				if (CYCLE.Symbol() == M25)
				{
					CondOutput( "G100"
								+ A.Uncond( clfile.Int( "feed" ) )
								+ B.Uncond( clfile.Int( "speed" ) )
								+ Z.AbsUncond( depth )
								+ ((toolType == Const.TAP) ? M23.Uncond() : M24.Uncond()) );
					CondOutput( "M64" );
				}

				CondOutput( X.AbsUncond( xe ) + Y.AbsUncond( ye ) );
				m_time += (2 * (depth / Feedrate()));
			}
		}
	}

	private void EndProgram()
	{
		String unloadPosition = ((OFFSET.Symbol() == G54)
					? X.AbsUncond( 139. ) + Y.AbsUncond(-35.)
					: X.AbsUncond( 119. ) + Y.AbsUncond(-35.));

		if (DEBUG) output.Dump( "====> EndProgram()" );
		
		CondOutput( M25.Cond() );
		
		CondOutput( G00.Uncond() + unloadPosition + M85.Uncond() );

		CondOutput( "M97" );
		CondOutput( "M28" );
//		CondOutput( "(CYCLE TIME" + CT.Uncond(m_time) + " MIN.)" );
		CondOutput( "M30" );
		output.Dump( "%" );
	}

	private void ToolChange()
	{
		int nextToolNo = NextToolNo();
		
		String toolDesc = "(" + ToolDesc() + ")";
		int toolType = clfile.Int( "Type_ID" );

		Disengage();

		if (DEBUG) output.Dump( "====> ToolChange()" );

		// CondOutput( M42.Cond() );
		CondOutput( M25.Cond() );

		m_currToolNo = ToolNo();

		if ( IsDrillTap( toolType ) )
		{
			// N57G101 M25
			// N59(ATTACHMENT)
			// N61M01
			// N63G105M05T098(SERVO DRILL)
			// N65G56(SERVO DRILL OFFSET)
			CondOutput( "(ATTACHMENT)" );
			CondOutput( "M01" );
			CondOutput( "G105M05" + T.Uncond( m_currToolNo ) + toolDesc );
			CondOutput( "G56(SERVO DRILL OFFSET)" );
			
			Z.Set(0.);
		}
		else
		{
			CondOutput( M85.Cond() );
			CondOutput( "G106" );
			
			if(m_IsFirstTool)
			{
				CondOutput( T.Uncond( m_currToolNo ));
				m_IsFirstTool=false;
			}

			CondOutput( T.Uncond( m_currToolNo ) + "M06 (" + ToolDesc() + ")" );

			if (IsValidToolNo( nextToolNo ) && (nextToolNo > 100))  // ie. not a drill/tap
				CondOutput( T.Cond( nextToolNo ) );

			if ( IsPunchTool() )  // ie. the current tool
				CondOutput( OffsetCode() ); 

			//CondOutput( PressureCode( m_currToolNo ) );
			m_time += 0.120; //.25 = 15 seconds    

			// if ( IsPunchTool() )
			//	Stripper();
		}
	}

	private void Engage()
	{
		String block;

		double xs = clfile.Xs() * m_cf;
		double ys = clfile.Ys() * m_cf;

		Point ps = new Point( X.Curr(), Y.Curr(), 0. );
		Point pe = new Point( xs, ys, 0. );

		boolean avoided = ConditionalClampsAvoid( ps, pe );

		if (DEBUG) output.Dump( "====> Engage()" );

		if ( !m_engaged )
		{
			if ( IsPunchTool() )
			{
				block = Rapid( xs, ys, false );
	
				if ( AutoIndex() )
					block += C.Cond( IndexAngle() );
	
				CondOutput( block );
			}
			else
			{
				double xe = clfile.Xe() * m_cf;
				double ye = clfile.Ye() * m_cf;
	
				// Assume some type of burner.
				int compSide = clfile.Int( "cutside" );
				CondOutput( OffsetCode() );
				CondOutput( Rapid( xs, ys, false ) );
				if (S.Curr() < 1.)
				{
					CondOutput( S.Uncond( SCode() ) + M17.Uncond() );
					CondOutput( F.Uncond( Feedrate() ) );
				}
				else
				{
					CondOutput( F.Cond( Feedrate() ) + M17.Cond() );
				}
				
				if (m_pierce == 1)
					CondOutput( "");
				
				block = "";	
				// block = G01.Cond() + X.AbsUncond( xe ) + Y.AbsUncond( ye );
	
				if (compSide != 0)
				{
					int doff = clfile.Int( "doff" );
					block += COMP.Uncond( compSide ) + D.Uncond( 1 );
				}
	
				CondOutput( block );
			}
		}
	
		m_engaged = true;
	}
	
	private void Disengage()
	{
		if (DEBUG) output.Dump( "====> Disengage()" );
		
		if ( m_engaged )
		{
			DbEntity dbEntity = clfile.Entity();
				
			CondOutput( M18.Cond() );

			if ( !m_dropstop )
			{			
				boolean need_stop = false;
				
				clfile.StatePush();
				clfile.Read();
				if (clfile.RecType() != Clfile.eDropCommand &&
				    clfile.RecType() != Clfile.eStopCommand)
		    	{
						need_stop = true;
		    	}
				clfile.StatePop();
				
				if (need_stop)
					CondOutput( "M00" );
					
				m_dropstop = false;
			}

			CondOutput( G40.Cond() );
		}
		
		m_engaged = false;
	}

	private boolean IsValidToolNo( int toolNo )
	{
		toolNo %= 100;
		
		return ((toolNo > 0 && toolNo < 16) || (toolNo >= 95 && toolNo <= 98));
	}

	private boolean IsPunchTool()
	{
		int type = clfile.Int( "Type_ID" );
		return IsPunchTool( type );
	}
	
	private boolean IsPunchTool( int type )
	{
		return ((type >= Const.ROUND && type <= Const.HEXAGON) || (type == Const.CUSTOM));
	}

	private boolean IsDrillTap()
	{
		int type = clfile.Int( "Type_ID" );
		return IsDrillTap( type );
	}
	
	private boolean IsDrillTap( int type )
	{
		return (type == Const.TAP || type == Const.DRILL);
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


	private double PunchLength()
	{
		double length = Const.UNDEFINED;

		DbEntity dbEntity = clfile.Entity();
		DbTool dbTool = dbEntity.Tool();

		if (dbTool != null)
		{
			if (dbTool.IntGet("Type_ID") == Const.ROUND)
			{
				length = 0.;
			}
			else
			{
				length = dbTool.DoubleGet( "length" );
				if (length >= Const.UNDEFINED)
					length = PunchWidth();
			}
		}

		return ((length <= Const.UNDEFINED) ? length : 0);
	}

	private double PunchWidth()
	{
		double width = Const.UNDEFINED;

		DbEntity dbEntity = clfile.Entity();
		DbTool dbTool = dbEntity.Tool();

		if (dbTool != null)
		{
			width = dbTool.DoubleGet( "width" );
			if (width >= Const.UNDEFINED)
			{
				width = dbTool.DoubleGet( "diameter" );
				if (width < Const.UNDEFINED)
					width /= 2;
			}
		}

		return ((width <= Const.UNDEFINED) ? width : 0);
	}	

	private void Stripper()
	{
		if (m_stripper == 0)
			CondOutput( "G177M55(NORMAL STRIPPER HEIGHT)" );
		else if (m_stripper == 1)
			CondOutput( "G177M54(EXTENDED STRIPPER HEIGHT)" );
		else
			CondOutput( "M56(TOOL CHANGE STRIPPER HEIGHT)" );
	}

	private void ConditionalSlugsDump()
	{
		if (m_hits < m_maxHits)
			return;

		CondOutput( "M20(SLUG DUMP AFTER " + m_hits + " HOLES)" );

		m_hits = 0;
	}

	private String OffsetCode()
	{
		String gcode = "";

		int type = clfile.Int( "Type_ID" );

		if ( IsPunchTool( type ) )
			gcode = G54.Uncond() + "(PUNCHING SEQUENCES FOLLOW)";
		else if (type == Const.BURNER)
			gcode = G55.Uncond() + "(PLASMA SEQUENCES FOLLOW)";
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
		int toolType = clfile.Int("Type_ID");

		if (toolType == Const.FORMING)
		{
			mcode = M41.Cond();
		}
		else
		{
			//int type = clfile.Int( "Type_ID" );

			//if (type == Const.FORMING)//( IsPunchTool( type ) )
			//	mcode = M41.Cond();
			//else if (type == Const.SCRIBE ||
			//	 type == Const.POWDER_MARK ||
			//	 type == Const.MARKING)
			//	mcode = M40.Cond();
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
		String CNCFileName = "viewer";
		String CNCFile = output.Path();
		if (CNCFile != null)
		{
			int PeriodPos = CNCFile.indexOf('.');
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

	private void Hold()
	{
		String	cmd;
		String	block;
		double	xs, ys;
		double	xe, ye;
		
		Disengage();

		if (DEBUG) output.Dump( "====> Hold()" );
		
		xs = X.Curr();
		ys = Y.Curr();
		
		xe = clfile.Xe() * m_cf;
		ye = (clfile.Ye() - Model.DoubleGet("Width")) * m_cf;
		
		if (ys > m_clamp[0].Ymin())
		{
			// Must clear clamp before moving to hold position.
			cmd = RapidTo( xs, ye );
			block = "(" + G00.Uncond() + X.AbsUncond(xs) + Y.AbsUncond(ye) + ")";
			
			output.Dump( SeqNum() + block );
		}
		
		cmd = RapidTo( xe, ye );
		block = G00.Uncond() + X.AbsUncond(xe) + L.Uncond(ye) + M85.Uncond();
		
		output.Dump( SeqNum() + block );
		CondOutput( "G106" );
	}
	
	private void Repo()
	{
		// NOTE: In this case, clfile.Xe() is an incremental distance
		// as extracted from the @REPO attribute 'x' parameter.

		if (DEBUG) output.Dump( "====> Repo()" );

		int repo = clfile.Int( "repo" );
		double repoLocation = clfile.Xe();
		double delta = repoLocation - G175X.Curr();
		
		CondOutput("(REPOSITION TO FOLLOW)");
		CondOutput( G175X.IncrUncond( repoLocation ));
		CondOutput( "M00 (CHECK CLAMPS)");
//		CondOutput( " Y" + m_repoLoad );

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
		// TODO: Generate a slide move as required.
		if (DEBUG) output.Dump( "====> Drop()" );

		CondOutput( M19.Cond() );

		if ( clfile.Dbl( "dx" ) < 7.0  && m_slidemovesmall == 1)
		{
			CondOutput("(SLIDE MOVE TO FOLLOW)");
			output.Dump( SeqNum() + G01.Uncond() + X.AbsCond( X.Curr() + 7.00 ) + "F400." );				
		}

		CondOutput( "M80" );
		CondOutput( "M01" );
		
		m_dropstop = true;
	}
	
	private void Stop()
	{
		if (DEBUG) output.Dump( "====> Stop()" );
		
		CondOutput( M18.Cond() );

		if ( clfile.Dbl( "dx" ) > 15.0 && m_slidemovelarge == 1) 
		{
			CondOutput("(SLIDE MOVE TO FOLLOW)");
			output.Dump( SeqNum() + G01.Uncond() + X.AbsCond( X.Curr() + 7.00 ) + "F400." );				
		}

		CondOutput( "M00" );
		
		m_dropstop = true;
	}

	private boolean HaveClamps()
	{
		return (m_clamp[0] != null && m_clamp[1] != null);
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
		
		// 2007.03.07 (PE) -- Frank feels better about using Y-7.
		lldy = -7.;
		
		for (indx = 0; indx < count; ++indx)
		{
			m_clamp[indx] = new SafeZone();

			attribName = "_clamp" + (indx+1) + "_x";
			xclamp = zone.DoubleGet( attribName );

			attribName = "_clamp" + (indx+1) + "_y";
			yclamp = zone.DoubleGet( attribName );

			m_clamp[indx].Set(
				xclamp, yclamp,
				(xclamp + lldx),
				(yclamp + lldy - width),
				(xclamp + trdx),
				(yclamp + trdy - width) );
		}
	}

	private boolean ConditionalClampsAvoid( Point ps, Point pe )
	{
		String comment = " (AVOIDING CLAMP)";
		boolean avoided = false;

		if ( !HaveClamps() || ps.WithinTol( pe, 1.e-3 ) )
			return avoided;  // early exit (not a nested sheet)

		if ( ps.WithinTol( pe, 1.e-3 ) )
			return avoided;  // not realistic.

		if ( DogLegCollision( ps, pe, " (AVOIDING CLAMP)" ) )
		{
			ps.Assign( X.Curr(), Y.Curr(), 0 );
			// output.Dump( "--- xs:" + ps.X() + "  ys:" + ps.Y() + "  xe:" + pe.X() + "  ye:" + pe.Y() );
			DogLegCollision( ps, pe, "" );
		}

		return avoided;
	}

	// For slide move ...
	private boolean ConditionalClampsAvoid2( Point ps, Point pe )
	{
		boolean avoided = false;

		if ( !HaveClamps() )
			return avoided;  // early exit (not a nested sheet)
		
		if ( m_clamp[0].Collision( ps, pe, 0. ) ||
			 m_clamp[1].Collision( ps, pe, 0. ) )
		{
			if (DEBUG) output.Dump( "====> ConditionalClampsAvoid2()" );

			if (ps.Y() > m_clamp[0].Ymin())
			{
				//CondOutput( "G04X0.5 (CLEARING CLAMP FOR SLIDE MOVE)" );
				avoided = true;
			}
		} 

		return avoided;
	}
	
	private boolean LineOfSightCollision( Point ps, Point pe )
	{
		return ( m_clamp[0].Collision( ps, pe, 0. ) ||
			 	 m_clamp[1].Collision( ps, pe, 0. ) );
	}
	
	private boolean DogLegCollision( Point ps, Point pe, String comment )
	{
		String	cmd;
		String	block;
		
		// NOTE: Y values are in 4th quadrant.		
		// 2/17/2003 -- X moves at 600 and Y moves at 1000
		if (ps.Y() > m_clamp[0].Ymin())
		{
			// Moving out of a clamp zone
			if ( CrossesClamps( ps, pe ) )
			{
				if (DEBUG) output.Dump( "====> DogLegCollision()" );
				
				cmd = RapidTo( ps.X(), m_clamp[0].Ymin() );
				block = Rapid( ps.X(), m_clamp[0].Ymin(), false ) + M85.Cond() + comment;
				output.Dump( SeqNum() + block );
				return true;
			}
		}
		else if (pe.Y() > m_clamp[0].Ymin())
		{
			// Moving into a clamp zone
			if ( CrossesClamps( ps, pe ) )
			{
				if (DEBUG) output.Dump( "====> DogLegCollision()" );
				
				cmd = RapidTo( pe.X(), ps.Y() );
				block = Rapid( pe.X(), ps.Y(), false ) + M85.Cond() + comment;
				output.Dump( SeqNum() + block );
				return true;
			}
		}
		
		return false;
	}

	private boolean CrossesClamps( Point ps, Point pe )
	{
		SafeZone	clamp;
		double		xs, xe;
		boolean		crosses;
		
		crosses = false;
		
		xs = ps.X();
		xe = pe.X();
		
		for (int indx = 0; indx < 2; ++indx)
		{
			clamp = m_clamp[indx];
			
			if ((xs < clamp.Xmin() && xe > clamp.Xmin()) ||
				(xe < clamp.Xmin() && xs > clamp.Xmin()) ||
				(xs < clamp.Xmax() && xe > clamp.Xmax()) ||
				(xe < clamp.Xmax() && xs > clamp.Xmax()))
			{
				return true;
			}
		}
		
		return false;
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
		sval = clfile.Str( "cg.pierce_delay" );
		m_pierce = (sval.equalsIgnoreCase("On") ? 1 : 0);

		// cg.slide_movesmall|combo|Small Part Slide|Off|On
		sval = clfile.Str( "cg.slide_movesmall" ).trim();
		m_slidemovesmall = (sval.equalsIgnoreCase("On") ? 1 : 0);

		// cg.slide_movelarge|combo|Large Part Slide|Off|On
		sval = clfile.Str( "cg.slide_movelarge" ).trim();
		m_slidemovelarge = (sval.equalsIgnoreCase("On") ? 1 : 0);

		// cg.torch_intensity|combo|Torch Intensity|M17|M18|M19
		m_torchCode = clfile.Str( "cg.torch_intensity" );

		// cg.stripper|combo|Stripper|Normal|Extended|Other
		sval = clfile.Str( "cg.stripper" );
		if (sval.compareToIgnoreCase("Normal") == 0)
			m_stripper = 0;
		else if (sval.compareToIgnoreCase("Extended") == 0)
			m_stripper = 1;
		else
			m_stripper = 2;

		// cg.load|combo|Material Loading|Manual|Automatic
		//sval = clfile.Str( "cg.load" );
		m_load = clfile.Str( "cg.load" );       

		// cg.xload|double|Load X|0.0
		// cg.yload|double|Load Y|0.0
		m_xLoad = clfile.Dbl( "cg.xload" );
		m_yLoad = clfile.Dbl( "cg.yload" );
//		m_Feed = Model.DoubleGet( "cg.Feed1" );
		m_repoLoad = clfile.Dbl( "cg.repo_load");

		m_Amps = clfile.Dbl( "cg.Amps" );

		m_engaged = false;
		
		m_clamp = new SafeZone[2];
		m_clamp[0] = null;
		m_clamp[1] = null;
	}
	
	private void TableVelocity()
	{
		// 0.454 pounds / kilogram
		double factor = (( m_metric ) ? 0.454 : 1.0);
		
		double materialWeight = clfile.Dbl( "MaterialWeight" ) * factor;

		if (materialWeight > 0 && materialWeight < (1000 * factor))
			m_tableVelocity = 1;
		else if (materialWeight >= (1001 * factor) && materialWeight < (1700 * factor))
			m_tableVelocity = 2;
		else if (materialWeight >= (1701 * factor) && materialWeight < (3300 * factor))
			m_tableVelocity = 3;
//		else if (materialWeight >= (1000 * factor))
//			m_tableVelocity = 4;
		else
			m_tableVelocity = 0;

		//if (m_tableVelocity >=1 && m_tableVelocity <= 3)
		//	CondOutput( "G150M6" + m_tableVelocity );
		//else if (m_tableVelocity == 0)
		//	CondOutput( "M00(RAMP RATES CANNOT BE DETERMINED !)" );
		//else
		//	CondOutput( "M00(MACHINE WEIGHT LIMITATION EXCEEDED !)" );
	}

	private int Feedrate()
	{
		double materialthick = Model.DoubleGet( "Thickness" );

		// return clfile.Int( "feed" );
		if ( materialthick <= .100)
			m_feed=125;

		else if ( materialthick > .100 && materialthick <= .105)
			m_feed =108;

		else if ( materialthick > .105 && materialthick <= .135)
			m_feed =100;

		else if ( materialthick > .135 && materialthick <= .200)
			m_feed =95;

		else if ( materialthick > .200 && materialthick <= .260)
			m_feed =90;

		else if ( materialthick > .260 && materialthick <= .375)
			m_feed =55;

		else if ( materialthick > .375 && materialthick <= .500)
			m_feed =50;

		else if ( materialthick > .500 && materialthick <= .625)
			m_feed =45;

		else if ( materialthick > .6255 && materialthick <= .750)
			m_feed =30;

		else if ( materialthick >= .750)
			m_feed =15;
			
		return (int) m_feed;
	}

	private int SCode()
	{
		int feed = Feedrate();

		switch (feed)
		{
		case  15:  return 250;
		case  30:  return 250;
		case  45:  return 250;
		case  50:  return 250;
		case  55:  return 240;
		case  75:  return 230;
		case  90:  return 220;
		case  95:  return 170;
		case 100:  return 130;
		case 108:  return 120;
		case 125:  return 100;
		case 130:  return 90;
		default:   return 0;
		}
	}

	// NOTE: This is the magic that causes display of
	// clamp avoidance movements.
	private String RapidTo( double xp, double yp )
	{
		// NOTE: type=0 is equivalent to type=GEOPOINT.
		String cmd =
			  "type=0"
			+ ", xp=" + UTIL.AbsUncond(xp)
			+ ", yp=" + UTIL.AbsUncond(yp);
			
		return ("\1" + cmd + "\2");
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
			Z = new DimAddr(  "Z", 5, 1, 4, 1, 1, 3, "0.0" );
			I = new MiscAddr( "I", 5, 1, 4, 1, 0, 3, "0.0" );
			J = new MiscAddr( "J", 5, 1, 4, 1, 0, 3, "0.0" );
			G175X = new DimAddr(  "G175X", 6, 1, 4, 1, 1, 3, "0" );
			L = new MiscAddr(  "Y", 6, 1, 4, 1, 1, 3, "0" );
		}
		else
		{
			X = new DimAddr(  "X", 5, 1, 3, 1, 1, 4, "0.0" );
			Y = new DimAddr(  "Y", 5, 1, 3, 1, 1, 4, "0.0" );
			Z = new DimAddr(  "Z", 5, 1, 3, 1, 1, 4, "0.0" );
			I = new MiscAddr( "I", 5, 1, 3, 1, 0, 4, "0.0" );
			J = new MiscAddr( "J", 5, 1, 3, 1, 0, 4, "0.0" );
			G175X = new DimAddr(  "G175X", 4, 1, 3, 1, 1, 4, "0" );
			L = new MiscAddr(  "Y", 6, 1, 4, 1, 1, 3, "0" );
		}
		
		K   = new MiscAddr( "K", 5, 0, 3, 0, 0, 0, "0" );

		N = new MiscAddr( "N", 0, 0, 4, 0, 0, 0, "0" );
		O = new MiscAddr( "O", 0, 2, 4, 0, 0, 0, "0000" );
		C = new MiscAddr( "C", 0, 0, 3, 0, 0, 2, "0" );
		T = new MiscAddr( "T", 0, 0, 3, 0, 0, 0, "0" );
		D = new MiscAddr( "D", 0, 0, 2, 0, 0, 0, "00" );
		F = new MiscAddr( "F", 0, 0, 3, 1, 0, 0, "0" );
		F2 = new MiscAddr( "#512=", 0, 0, 5, 1, 2, 0, "0" );
		S = new MiscAddr( "S", 0, 2, 3, 0, 0, 0, "000" );
		A = new MiscAddr( "A", 0, 0, 5, 1, 0, 1, "0" );  // drill/tap feedrate
		B = new MiscAddr( "B", 0, 0, 5, 1, 0, 1, "0" );  // drill/tap spindle speed
		CT = new MiscAddr( " ", 5, 0, 5, 1, 0, 2, "0" );
		UTIL = new DimAddr( "", 5, 1, 3, 1, 1, 4, "0." );


		UNITS  = new Group( null );
		ABSINC = new Group( null );
		COMP   = new Group( null );
		TOR   = new Group( null );  //   4/6/01

		PRESS  = new Group( null );
		OFFSET = new Group( null );
		STOP   = new Group( null );
		PUNCH  = new Group( null );
		LOAD   = new Group( null );
		PIERCE = new Group( null );
		MOTION = new Group( null );
		LINEAR = new Group( MOTION );
		ARC    = new Group( MOTION );
		CYCLE  = new Group( null );

		G00 = new Symbol( LINEAR, "G00", Clfile.eRapid );
		G01 = new Symbol( LINEAR, "G01", Clfile.eLine );
		G02 = new Symbol( ARC,    "G02", Clfile.eCwArc );
		G03 = new Symbol( ARC,    "G03", Clfile.eCcwArc );
		G76 = new Symbol( LINEAR, "G76", 99 );  // Arbitrary (99)
		
		M25 = new Symbol( CYCLE,  "G101M25", 0 );
		M23 = new Symbol( CYCLE,  "M23", 1 );
		M24 = new Symbol( CYCLE,  "M24", 2 );

		G54 = new Symbol( OFFSET, "G54",  0 );	// activate punch offset
		G55 = new Symbol( OFFSET, "G55",  1 );	// activate torch offset
		G56 = new Symbol( OFFSET, "G56",  2 );	// activate tap offset
		G57 = new Symbol( OFFSET, "G57",  3 );	// activate marking offset

		G20 = new Symbol( UNITS,  "G20",  0 );	// inch
		G21 = new Symbol( UNITS,  "G21",  1 );	// metric

		G40 = new Symbol( COMP,   "G40",  0 );	// off
		G41 = new Symbol( COMP,   "G41",  1 );	// left
		G42 = new Symbol( COMP,   "G42", -1 );	// right

		M17 = new Symbol( TOR,   "M17",  0 );	// TORCH ON  4/6/01
		M18 = new Symbol( TOR,   "M18",  1 );	// TORCH OFF DROP DOOR CLOSED 4/6/01
		M19 = new Symbol( TOR,   "M19", -1 );	// TORCH OFF DROP DOOR OPEN 4/6/01


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
		M170 = new Symbol( LOAD,   "M170(AUTO LOAD)",  2 );  

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

	boolean m_dropstop = false;		// state info
	
	String	m_torchCode		= null;
	String	m_load			= null;    
	boolean	m_engaged		= false;
	boolean	m_IsFirstTool	= true;
	double	m_xLoad			= 0.0;
	double	m_yLoad			= 0.0;
	double	m_Amps			= 0.0;
	double	m_repoLoad			= 0.0;


	int		m_tableVelocity	= 0;
	int		m_pierce		= 0;
	int		m_slidemovesmall	= 0;
	int		m_slidemovelarge	= 0;
	int		m_stripper		= 0;

	int		m_maxHits	= 100;
	int		m_hits		= 0;

	double	m_feed = Const.UNDEFINED;
	
	DimAddr  X = null;
	DimAddr  Y = null;
	DimAddr  Z = null;
	MiscAddr I = null;
	MiscAddr J = null;
	MiscAddr K = null;
	MiscAddr C = null;
	MiscAddr N = null;
	MiscAddr O = null;
	MiscAddr T = null;
	MiscAddr D = null;
	MiscAddr F = null;
	MiscAddr F2 = null;
	MiscAddr S = null;
	MiscAddr A = null;
	MiscAddr B = null;
	MiscAddr CT = null;
	DimAddr G175X = null;
	MiscAddr L = null;
	DimAddr  UTIL = null;



	Group UNITS  = null;
	Group TOR  = null;
	Group ABSINC = null;
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
	Group CYCLE  = null;

	Symbol G00 = null;
	Symbol G01 = null;
	Symbol G02 = null;
	Symbol G03 = null;
	Symbol G76 = null;

	Symbol M25 = null;
	Symbol M23 = null;
	Symbol M24 = null;

	Symbol G54 = null;
	Symbol G55 = null;
	Symbol G56 = null;
	Symbol G57 = null;

	Symbol G20 = null;
	Symbol G21 = null;

	Symbol G40 = null;
	Symbol G41 = null;
	Symbol G42 = null;

	Symbol M17 = null;  //  4/6/01
	Symbol M18 = null;  //  4/6/01
	Symbol M19 = null;  //  4/6/01

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

