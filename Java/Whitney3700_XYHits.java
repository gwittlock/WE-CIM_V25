// cg.prog|int|Program Number|1234
// cg.units|dcombo|Units|Inch|Inch|Metric|Inch to Metric|Metric to Inch
// cg.load|dcombo|Material Loading|M87(MANUAL)|M87(MANUAL)|M88(AUTOMATIC)|M170(PLATEPARTNER)
// cg.xload|dbl|Load X|0.0
// cg.yload|dbl|Load Y|-38.0
// cg.delay|dcombo|Pierce Delay|On|On|Off
// cg.switch|dcombo|G61/G4|G64|G61|G64
// cg.slide_movelarge|dcombo|Large Part Slide|Off|Off|On
// cg.slide_doorlarge|dcombo|Open Door|None|None|Before Slide|After Slide
// cg.slide_distlarge|dbl|  Slide Distance|0
// cg.rapid|dbl|Rapid rate|100


import Weng.System.*;
import Weng.Modeler.*;
import Weng.Math.*;
import Weng.Geometry.*;
import Weng.CodeGen.*;
import java.io.*;
import java.util.*;

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Whitney3700 for Morton Mfg
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// 2005.04.27 (PE) -- Initial version.
// 2005.07.26 (PE) -- Fixed problem with clamp avoidance.  The
//   entire clamp was shifted by CLAMP_DELTA_Y, instead of just
//   lower edge being shifted.
// 2007.01.16 (PE) -- Prevented attempt to reload tool at end of
//   program when the program uses only one tool. Otherwise, it
//   causes the machine to err/stall.
// 2007.03.13 (PE) -- Output pressure code based on tool type.
// 2008.02.05 (PE) -- Force output of X & Y at first hole after a toolchange.
//
public class Whitney3700_XYHits implements WengMacro
{
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Frequently modified variables & constants.

	private final boolean DEBUG = false;
	private final boolean REC = false;

	private double RAPID_RATE = 2000.;		// ipm (previously a constant)
	private final double DROP_DOOR_TIME = 2.5 / 60.;
	private final double TOOL_CHANGE_TIME = 6.5 / 60.;
	private final double PUNCH_HIT_TIME = 0.4 / 60.;
	private final double BURN_START_TIME = 0.5 / 60.;

	private final double CLAMP_DELTA_Y = 1.0;

	// The next three values come straight from Whitney3700 blueprint.
	private final double DROP_DOOR_DX = 18.039;
	private final double DROP_DOOR_DY = 24.09;
	// dist between torch center and right edge of drop door
	private final double DROP_OFFSET = 1.016;
	// comfort factor to help ensure part will drop
	private final double DROP_SLOP = 0.1;

	//private final double LANDING_DELTA = 2.5;
	private final double LANDING_DELTA = 1.5;
	
	// Distance to move right edge of part past edge of drop door.
	private final double SLIDE_DELTA = 0.25;

	// Parts whose dx is less than this value require a slide move
	// to slide the part off of the landing.
	private final double SLIDE_THRESHOLD = 5.0;
	
	// Distance to move past bottom of sheet.
	private final double DRILL_DELTA = 0.375;
	
	// Y distance to move away from clamp for slide move.
	private final double CLAMP_CLEARANCE = 6.0;

	// Per Alex's instructions.
	private final double PUNCH_OVERLAP = 0.032;
	
	private boolean m_useSeqNum = true;
	private int m_seqNum = 1;
	private int m_seqNumInc = 2;

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// BEGIN Inner Class Definition
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
		protected class Xref
		{
			public Xref(
					double	material_thickness,
					double	feedrate,
					double	pierce_time )
			{
				m_material_thickness = material_thickness;
				m_feedrate = feedrate;
				m_pierce_time = pierce_time;
			}
			
			public double MaterialThickness()	{ return m_material_thickness; }
			public double Feedrate()			{ return m_feedrate; }
			public double PierceTime()			{ return m_pierce_time; }
			
			private double	m_material_thickness;
			private double	m_feedrate;
			private double	m_pierce_time;
		}
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// END Inner Class Definition
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// The main processing loop.
	//
	public void main()
	{
		int fileStatus;
		
		try
		{
			Initialize();
			
			fileStatus = output.Open();

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
					Disengage();
					CondOutput( M18.Uncond() );
					CondOutput("M01");
					CondOutput( G54.Uncond());
					CondOutput( G00.Uncond()
						+ G70.Uncond()
						+ X.AbsUncond( clfile.Xe() )
						+ L.Uncond( clfile.Ye() - Model.DoubleGet("Width") ) );
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
		Xref	record;
		double	thickness;
		int		count, indx;
		
		clfile = new Clfile();
		output = new OutSys();

		ClassVarsInit();
		SymbolsInit();
		G40.Uncond();
		M41.Uncond();
		
		G175X.Set(0.);
		
		ClampsInit();
		
		thickness = Model.DoubleGet("Thickness");
		
				

/*		
		// NOTE: Be sure these records are added in increasing
		// order of material thickness.
		//   eg. new Xref( material_thickness, feedrate, pierce_time )
		//
		// where:
		//   feedrate -- ipm
		//   pierce_time -- sec
		//
		//Vector<Double> ptsX = new Vector<>();
		
		Vector<String> m_table = new Vector<String>();
		m_table.add( new Xref( 0.075,  260.,  0.1 ) );
		m_table.add( new Xref( 0.105,  250.,  0.1 ) );
		m_table.add( new Xref( 0.135,  240.,  0.1 ) );
		m_table.add( new Xref( 0.164,  210.,  0.1 ) );
		m_table.add( new Xref( 0.179,  180.,  0.1 ) );
		m_table.add( new Xref( 0.187,  180.,  0.1 ) );
		m_table.add( new Xref( 0.250,  160.,  0.1 ) );
		m_table.add( new Xref( 0.375,  100.,  0.1 ) );
		m_table.add( new Xref( 0.500,   80.,  0.1 ) );
		m_table.add( new Xref( 0.625,   70.,  0.1 ) );
		
		
		thickness = Model.DoubleGet("Thickness");
		
		count = m_table.size();
		for (indx = 0; indx < count; ++indx)
		{
			record = (Xref) m_table.elementAt(indx);
			
			// NOTE: We add 0.005 as tolerance for comparison.
			if ((record.MaterialThickness() + 0.005) >= thickness)
				break;
		}
		
		if (indx >= count)
			indx = count - 1;  // we have exhausted the listed materials
			
		record = (Xref) m_table.elementAt(indx);
*/
		//m_feedrate = Model.DoubleGet("FeedRate");
		m_pierce_time = 0.1;
		
		m_first_tnum = 0;
	}

	private void Finalize()
	{
		clfile = null;
		output = null;
	}
  
	private void ToolReport()
	{
		String	fname;
		int []	toolNos = new int[50];
		int		indx, count = 0;
		
		m_tool_count = 0;
		
		clfile.Read();

		fname = CNCFile();
		
		output.Dump(" ");
		output.Dump( O.Uncond( Model.IntGet("cg.prog") ) );
			
		output.Dump( "(FILE -- " + fname + ")" );
		output.Dump( TimeFormat() );
		output.Dump( "(CODED FOR 3700ATC-FR TRUECUT)" );
		output.Dump( "(------ BEGIN TOOL REPORT ------)" );
		
		while ( !clfile.AtEnd() )
		{
			clfile.Read();
			if (clfile.RecType() == Clfile.eToolChangeEnd)
			{
				DbEntity dbEntity = clfile.Entity();
				DbTool dbTool = dbEntity.Tool();
				
			    int toolNo = dbTool.IntGet("NC_Code_Number");

				if ( true )  // IsValidToolNo( toolNo ) )
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

						if (dbTool.IntGet("Type_ID") != Const.BURNER)
							++m_tool_count;
							
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
		String	delay;
		String	load;
		Double thickness;
		int nextToolNo = NextToolNo();

		if (DEBUG) output.Dump( "====> StartProgram()" );
		
		did_repo = false;

		/*
		output.Dump( "(CG VER. DATE 6/8/2000)" );
		output.Dump( "(MATERIAL: " + clfile.Str( "MatCfg" ) + ")" );
		output.Dump( "(LENGTH: " + clfile.Str( "Length" ) + ")" );
		output.Dump( "(WIDTH: " + clfile.Str( "Width" ) + ")" );
		output.Dump( "(THICKNESS: " + clfile.Str( "Thickness" ) + ")" );
		*/
		
		CondOutput( UNITS.Uncond( (m_metric ? 1 : 0) ) );
		CondOutput( ABSINC.Uncond( (m_incr ? 1 : 0) ) );
		CondOutput( G40.Uncond() );
		CondOutput( G54.Uncond() );
		CondOutput( "M31" );
		
		delay = Model.StringGet("cg.delay").trim();
		if ( delay.equalsIgnoreCase("On") )
		{
			CondOutput( "M65 (PIERCE DELAY ON)" );
		}

		CondOutput( "M46" );

		if (nextToolNo != UNDEFINED)
			CondOutput( T.Uncond( nextToolNo ) );

		CondOutput( MODE.Uncond( m_mode ) );
		
		thickness = Model.DoubleGet("Thickness");
		
		if(thickness >= 0.075)
		{
			m_feedrate = 260;
		}

		else if(thickness >= 0.105)
		{
			m_feedrate = 250.;
		}
		
		else if(thickness >= 0.135)
		{
			m_feedrate = 240.;
		}
		
		else if(thickness >= 0.164)
		{
			m_feedrate = 210;
		}
		
		else if(thickness >= 0.179)
		{
			m_feedrate = 180.;
		}
	
		else if(thickness >= 0.250)
		{
			m_feedrate = 160.;
		}
		
		else if(thickness >= 0.375)
		{
			m_feedrate = 100.;
		}
		
		else if(thickness >= 0.500)
		{
		
			m_feedrate =  80.;
		}
		
		else if(thickness >= 0.625)
		{
			m_feedrate =  70.;
		}
		
		else
		{
			m_feedrate =0.1;
		}
		
		//CondOutput(" THE MATERIAL THICKNESS IS < " + Model.DoubleGet("Thickness"));
		
		
		CondOutput( "#512=" + F.Uncond( Feedrate() ) + " (SET FEEDRATE)" );  // TODO
	  	CondOutput( G00.Uncond() + M85.Uncond() );
		// CondOutput( "M100 A"+ Model.StringGet("cg.amps") + " D"+ Model.StringGet("cg.delay") );

		TableVelocity();

		if ( HaveClamps() )
		{
			CondOutput( "G178"
						+ X.AbsUncond( m_clamp[0].X() )
						+ Y.AbsUncond( m_clamp[1].X() )
						+ "(CLAMP SETTINGS)" );
		}
		
		load = Model.StringGet( "cg.load" ).trim();
		if ( load.equalsIgnoreCase("M88(AUTOMATIC)") )
			CondOutput( X.AbsUncond( m_xLoad ) + Y.AbsUncond( m_yLoad ) + "M88 (AUTOMATIC)" );
		else if ( load.equalsIgnoreCase("M87(MANUAL)") )
			CondOutput( X.AbsUncond( m_xLoad ) + Y.AbsUncond( m_yLoad ) + "M87 (MANUAL)" );
		else if ( load.equalsIgnoreCase("M170(PLATEPARTNER)") )
			CondOutput( X.AbsUncond( m_xLoad ) + Y.AbsUncond( m_yLoad ) + "M170 (PLATEPARTNER)" );
		
		C.Uncond( 180. );  // head starts at 180.
	}
	
	private String TimeFormat()
	{
		Calendar calendar = Calendar.getInstance();
		Date date = calendar.getTime();
		String time = date.toString();
		int day   = calendar.get(java.util.Calendar.DATE);
		int month = calendar.get(java.util.Calendar.MONTH) + 1;
		int year  = calendar.get(java.util.Calendar.YEAR);
		int hour  = calendar.get(java.util.Calendar.HOUR_OF_DAY);
		int min   = calendar.get(java.util.Calendar.MINUTE);
		
		return ( "(" + hour + ":" + min + " -- " + month + "/" + day + "/" + year + ")" );
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
				block = G00.Uncond() + X.AbsCond( xe ) + Y.AbsCond( ye );
		}

		return block;
	}

	private void Line()
	{
		double xe = clfile.Xe() * m_cf;
		double ye = clfile.Ye() * m_cf;
		
		if (DEBUG) output.Dump( "====> Line()" );
		
		Line line = clfile.Line( );
		m_time += (line.ArcLen( ) / Feedrate());

		if ( IsPunchTool() )
		{
			Point		ps;
			UnitVec2d	vec;
			double		punch_len;
			double		dx, dy;
			int			count, indx;
						
			punch_len = PunchLength() - PUNCH_OVERLAP;
			
			ps = line.StartPt();
			vec = line.StartTan();
			
			dx = punch_len * vec.X();
			dy = punch_len * vec.Y();
			
			count = ((int) (line.ArcLen() / punch_len)) + 1;
			for (indx = 0; indx < count; ++indx)
			{
				CondOutput( M75.Cond() );
				
			Point ps2 = new Point( X.Curr(), Y.Curr(), 0. );
			Point pe = new Point( xe, ye, 0. );

			boolean avoided = ConditionalClampsAvoid( ps2, pe );

				xe = (ps.X() + (dx * indx)) * m_cf;
				ye = (ps.Y() + (dy * indx)) * m_cf;

				CondOutput( X.AbsUncond( xe ) + Y.AbsUncond( ye ) );

				++m_hits;

				m_time += PUNCH_HIT_TIME;

				ConditionalSlugsDump();
			}
		}
		else
		{
			Engage();
	
			if ( X.Delta( xe ) || Y.Delta( ye ) )
			{
				String block;
	
				if ( m_incr )
					block = G01.Cond() + X.IncrUncond( xe ) + Y.IncrUncond( ye );
				else
					block = G01.Cond() + X.AbsCond( xe ) + Y.AbsCond( ye );
	
				if (COMP.Symbol() != G40 && clfile.Entity().IsLeadOut())
					block += (G40.Uncond() + D.Uncond(0));
	
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

				block = X.AbsCond( xe ) + Y.AbsCond( ye );
				//if ( AutoIndex() )
				//	block += C.Cond( IndexAngle() );

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
		
		if ((m_tool_count > 1) && (m_first_tnum > 0))
		{
			// Per Ryan's instructions, this is how the operators
			// prevent the program from stalling (an intermittent problem).
			CondOutput( T.Uncond( m_first_tnum ) );
		}
		
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
		
		// CondOutput( M42.Cond() );

		CondOutput( "G106" );
		CondOutput( M85.Cond() );

		m_currToolNo = ToolNo();
		CondOutput( T.Uncond( m_currToolNo ) + "M06 (" + ToolDesc() + ")" );
		// CondOutput( "( future tool comment )" );

		if (nextToolNo != UNDEFINED)
		{
			CondOutput( "()" );
			if (nextToolNo == 108)
			{
				CondOutput(
					  T.Cond( nextToolNo )
					+ " (FETCH DUSTCOVER FOR PLASMA)"  );  
			}
			else
			{
				CondOutput(
					  T.Cond( nextToolNo )
					+ " (FETCH " + T.Uncond( nextToolNo ) + ")"  );  
			}
		}
			
		CondOutput( OffsetCode() );   
		
		CondOutput( PressureCode( m_currToolNo ) );

		if ( IsPunchTool() )
		{
			if (m_first_tnum == 0)
				m_first_tnum = m_currToolNo;  // See also EndProgram.
				
			Stripper();
			C.Uncond( 180. );  // G106 resets head to 180.
			
			// 2008.02.05 (PE) -- These next statements will cause the X & Y
			// to be forced out at the first hit. This is necessary because
			// (prior to this change) if you had the sequence ptA-toolchange-ptA,
			// the coordinates of the second instance of ptA would not be output.
			X.Set( X.Curr() + 0.001 );
			Y.Set( Y.Curr() + 0.001 );
		}

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
	
				//if ( AutoIndex() )
				//	block += C.Cond( IndexAngle() );
	
				CondOutput( block );
			}
			else
			{
				double dx, dy;
				
				double xe = clfile.Xe() * m_cf;
				double ye = clfile.Ye() * m_cf;
	
				// Assume some type of burner.
				int cutside = clfile.Int( "cutside" );
	
				CondOutput( OffsetCode());
				CondOutput( G00.Uncond() + X.AbsUncond( xs ) + Y.AbsUncond( ys ));//Rapid( xs, ys, false ));
				// CondOutput( "F#512" + M17.Uncond() + G61.Cond() );
				CondOutput( "F#512" + M17.Uncond() + MODE.Cond( m_mode ) );
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
			CondOutput("G40D00");
			if ( !m_dropstop )
			{
				CondOutput( M18.Cond() );  // turn off torch
			}
			
			m_dropstop = false;
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
		return ((type >= Const.ROUND && type <= Const.HEXAGON) ||(type == Const.CUSTOM));

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
		final double TOL = 1.e-2;
		
		int type = clfile.Int( "Type_ID" );
		double currAng = C.Curr();
		double nextAng = clfile.Dbl( "orient" );
		double delta = Math.abs( nextAng - currAng );

		if (delta > TOL)
		{
			// Limit head rotations by considering tool symmetry.
			switch (type)
			{
			case Const.RECTANGLE:
			case Const.OBROUND:
			case Const.DIAMOND:
			case Const.DOUBLE_D:
			case Const.KEYHOLE:
				if (Math.abs(delta - 180.) < TOL)
				{
					nextAng = currAng;
				}
				break;
				
			case Const.SQUARE:
				if ( (Math.abs(delta - 270.) < TOL) ||
					 (Math.abs(delta - 180.) < TOL) ||
					 (Math.abs(delta -  90.) < TOL)  )
				{
					nextAng = currAng;
				}
				break;
				
			case Const.HEXAGON:
				if ( (Math.abs(delta - 300.) < TOL) ||
					 (Math.abs(delta - 240.) < TOL) ||
					 (Math.abs(delta - 180.) < TOL) ||
					 (Math.abs(delta - 120.) < TOL) ||
					 (Math.abs(delta -  60.) < TOL)  )
				{
					nextAng = currAng;
				}
				break;
				
			default:
				break;
			}
		}

		if (nextAng < 0)
			nextAng += 360.;
			
		return nextAng;
	}

	private void Stripper()
	{
		int type = clfile.Int( "Type_ID" );

		if (type == Const.FORMING)
		{
			CondOutput("M56 (FULL STRIPPER HEIGHT)");
		}
		else
		{

			CondOutput( G177M55.Uncond() + " (NORMAL STRIPPER HEIGHT)" );	
		}			
	}

	private void ConditionalSlugsDump()
	{
		/*	
		if (m_hits < m_maxHits)
			return;
			
		if (DEBUG) output.Dump( "====> ConditionalSlugsDump()" );

		CondOutput( "M20(SLUG DUMP AFTER " + m_hits + " HOLES)" );
		*/
		m_hits = 0;
	}

	private String OffsetCode()
	{
		String gcode = "";

		int type = clfile.Int( "Type_ID" );

		if ( IsPunchTool( type ) )
			gcode = G54.Uncond() + " (PUNCHING SEQUENCES FOLLOW)";
		else if (type == Const.BURNER)
			gcode = G55.Uncond(); // + "(PLASMA SEQUENCES FOLLOW)";
		else if (type == Const.TAP)
			gcode = G56.Uncond() + " (TAPPING SEQUENCES FOLLOW)";
		else if (type == Const.SCRIBE ||
				 type == Const.POWDER_MARK ||
				 type == Const.MARKING)
			gcode = G57.Uncond() + " (MARKING SEQUENCES FOLLOW)";

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
		else
		{
			mcode = M42.Cond();
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
		
		String sval = clfile.Str("Description").toUpperCase();
		double dval = clfile.Dbl("Index_Angle");
		int type = clfile.Int( "Type_ID" );
		
		if (sval.length() > 0)
			toolDesc = sval;
	
		if (IsPunchTool(type) && (dval >= 0.) && (dval < 360.))
			toolDesc = toolDesc + " " + ORIENT.Uncond(dval) + " DEG";
			
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
				
		CondOutput("(REPOSITION TO FOLLOW)");
		CondOutput( G175X.IncrUncond( repoLocation ) + "Y-.05" );

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
		DbEntity	owner;
		Box3d		box;
		Point		ps;
		Point		pe;
		String		type;
		double		dxA, dxB;
		double		slide_dx;
		double		slide_dy;
		double		xnext = 0.;
		double		ynext = 0.;
		double		dx = 0.;
		boolean		needSlide = false;
		boolean		needDrop = true;
		
		if (DEBUG) output.Dump( "====> Drop()" );

		slide_dx = clfile.Entity().DoubleGet("_slide_dx");
		slide_dy = clfile.Entity().DoubleGet("_slide_dy");
		
		if ((slide_dx < Const.UNDEFINED) &&
			(slide_dy < Const.UNDEFINED))
		{
			xnext = X.Curr() + slide_dx;
			ynext = Y.Curr() + slide_dy;
			needSlide = true;
		}
		else
		{
			// Get the bounding box of the part.			
			owner = clfile.Entity().Owner();
			if (owner.Type() == Const.FEATURE)
			{
				type = owner.StringGet("type");
				if (type != null)
				{
					type = type.toLowerCase();
					if (type.indexOf("_lead") == 0)
						owner = owner.Owner();
				}
			}
			
			box = owner.Box();
			
			// ASSUMPTION: The lead-out is at (or very near)
			// the right edge of the part.  Also, (and this
			// really goes without saying it) the end of the
			// lead-out is positioned under the torch.
			
			// delta to left edge of part.
			dxA = X.Curr() - box.Xmin();
			
			if (box.Dx() < SLIDE_THRESHOLD)
			{
				// We have encountered either a small part or
				// a relatively small part whose lead-out is
				// not on the right edge of the part.
				
				// So that we slide the part off of the landing.
				xnext = X.Curr() + LANDING_DELTA;
				ynext = Y.Curr();
				
				needSlide = true;
			}
			else
			{
				// Now (because of oversized parts), we try to move the
				// left edge of the part to the left edge of the drop door.
	
				// NOTE: The following statement is incorrect. The correct
				// statement is dxB = DROP_DOOR_DX + DROP_OFFSET - DROP_SLOP;
				// However, since the operators are not complaining, the
				// incorrect statement remains active :-(
					
				// delta to left edge of door.
				dxB = DROP_DOOR_DX - DROP_OFFSET - DROP_SLOP;
				
				if (DEBUG)
				{
					output.Dump( "xcurr:" + X.Curr() );
					output.Dump( "box.Xmin:" + box.Xmin() );
					output.Dump( "box.Xmax:" + box.Xmax() );
					output.Dump( "delta to left edge of part:" + dxA );
					output.Dump( "delta to left edge of door:" + dxB );
				}
				
				if (dxA > dxB)
				{
					// The distance between the torch and the left
					// edge of the part is greater than the distance
					// between the torch and the left edge of the
					// drop door.
				
					dx = dxA - dxB;
					
					if (DEBUG) output.Dump( "delta to move:" + (-dx) );
					
					xnext = X.Curr() - dx;
					ynext = 0.5 * (box.Ymax() + box.Ymin());
					
					needSlide = false;
				}
			}
		}
		
		CondOutput( G40.Cond() );
		CondOutput( M18.Uncond() );

		if ( needSlide )
		{
			CondOutput(" (SLIDE MOVE TO FOLLOW)");
			CondOutput(
				  G01.Uncond()
				+ X.AbsCond( xnext )
				+ Y.AbsCond( ynext )
				+ "F200." );
			
			m_time += (dx / 400.);
		}
		
		if ( needDrop )
		{
			m_time += DROP_DOOR_TIME;
			CondOutput( "M80 (PLASMA TRAP DOOR)" );
			CondOutput( "M01" );
		}
	
		m_dropstop = true;
	}
	
	private void Stop()
	{
		if (DEBUG) output.Dump( "====> Stop()" );

		String slide = Model.StringGet( "cg.slide_movelarge" ).trim();
		String door = Model.StringGet( "cg.slide_doorlarge" ).trim();
		double dist = Model.DoubleGet( "cg.slide_distlarge" );
		
		CondOutput( M18.Uncond() );

		if ( door.equalsIgnoreCase("Before Slide") )
			CondOutput( "M80 (Plasma Trap Door)" );
		
		if ( slide.equalsIgnoreCase("On") && Math.abs( dist ) > 1.e-3 )
		{
			CondOutput(" (SLIDE MOVE TO FOLLOW)");
			output.Dump( SeqNum() + G01.Uncond() + X.AbsCond( X.Curr() + dist ) + "F200." );				
			
			m_time += (dist / 400.);
		}

		if ( door.equalsIgnoreCase("After Slide") )
			CondOutput( "M80 (Plasma Trap Door)" );
		
		CondOutput( "M00" );
		
		m_dropstop = true;
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
		
		width = Model.DoubleGet("Width") + CLAMP_DELTA_Y;
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
				(xclamp + trdx), 1. );
		}
	}

	private boolean ConditionalClampsAvoid( Point ps, Point pe )
	{
		String comment = " (AVOIDING CLAMP)";
		boolean avoided = false;

		// Per email from Ryan (2004.11.22) -- "When a tool is loaded
		// in the ram, it CANNOT rapid travel over a clamp, but when
		// the dust cover is loaded it CAN travel over the clamps.
		// Therefore when burning, the dust cover is automatically
		// loaded in the ram and clamp avoidance is not needed.  By
		// taking this out of the programs would save time and the
		// occasional bending of an already thin skeleton.  When we
		// notice the skeleton is thin enough and it might bend by
		// avoiding the clamps I will remove them from the program.
		//if (IsPunchTool() == false)
		//	return avoided;

		if (did_repo == true)
		{
			// Per email from Ryan (2004.11.22) -- "... here you will
			// notice the machine just performed a G175 so it is in
			// a safe tool load position.  After the tools change the
			// machine performs a G70 for the clamp avoidance.  It
			// does not need to do this operation.
			did_repo = false;
			return false;	
		}
		
		if ( !HaveClamps() || ps.WithinTol( pe, 1.e-3 ) )
			return avoided;  // early exit (not a nested sheet)

		if ( LineOfSightCollision( ps, pe ) )
		{
			if (DEBUG) output.Dump( "====> ConditionalClampsAvoid(): Line of Sight" );

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
		else if ( DogLegCollision( ps, pe ) )
		{
			if (DEBUG) output.Dump( "====> ConditionalClampsAvoid(): Dog Leg" );

			CondOutput( G70.Uncond() + Rapid( pe.X(), ps.Y(), false ) + comment );
		}

		return avoided;
	}
	
	private boolean LineOfSightCollision( Point ps, Point pe )
	{
		if (DEBUG) output.Dump( "====> Line of Sight" );
		
		return ( m_clamp[0].Collision( ps, pe, 0. ) ||
			 	 m_clamp[1].Collision( ps, pe, 0. ) );
	}
	
	private boolean DogLegCollision( Point ps, Point pe )
	{
		boolean dogLeg = false;
		
		if (DEBUG) output.Dump( "====> DogLegCollision" );
		
		// NOTE: We are only concerned with a collision when moving towards
		// the clamps because the machine traverses first along some
		// 45 degree vector before moving strictly along the other axes.

		// ASSUMPTION: Both clamps have the same Y extents.
		if (pe.Y() > m_clamp[0].Ymin())
		{
			Point pt;	// the intermediate point
			double dx, dy, dx2;
			double xp, yp;
			
			dx = pe.X() - ps.X();
			dy = Math.abs( pe.Y() - ps.Y() );
			
			if (Math.abs( dx ) > dy)
			{
				dx2 = dy * Math.tan( Const.QUARTERPI );
				
				xp = ps.X() + ((dx < 0) ? -dx2 : dx2);
				yp = pe.Y();
				
				pt = new Point( xp, yp, 0. );
				
				dogLeg = LineOfSightCollision( ps, pt );
			}
		}
		
		return dogLeg;
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
		// sval = Model.StringGet( "cg.pierce_delay" ).trim();
		// m_pierce = ((sval.compareToIgnoreCase("On") == 0) ? 1 : 0);
		
		sval = Model.StringGet( "cg.switch" ).trim();         
		m_mode = (( sval.equalsIgnoreCase("G61") ) ? 0 : 1);

		// cg.xload|double|Load X|0.0
		// cg.yload|double|Load Y|0.0
		m_xLoad = Model.DoubleGet( "cg.xload" );
		m_yLoad = Model.DoubleGet( "cg.yload" );

		m_clamp = new SafeZone[2];
		m_clamp[0] = null;
		m_clamp[1] = null;
		
		RAPID_RATE = Model.DoubleGet( "cg.rapid" );
		
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
		return m_feedrate;	// ipm
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
			X = new DimAddr(  "X", 5, 1, 3, 1, 1, 3, "0.0" );
			Y = new DimAddr(  "Y", 5, 1, 3, 1, 1, 3, "0.0" );
			I = new MiscAddr( "I", 5, 1, 3, 1, 0, 3, "0.0" );
			J = new MiscAddr( "J", 5, 1, 3, 1, 0, 3, "0.0" );
			G175X = new DimAddr(  "G175X", 6, 1, 3, 1, 1, 4, "0" );
			L = new MiscAddr(  "Y", 6, 1, 4, 1, 1, 3, "0" );
		}

		O = new MiscAddr( ":O", 0, 2, 4, 0, 0, 0, "0000" );
		N = new MiscAddr(  "N", 0, 0, 4, 0, 0, 0, "0" );
		C = new MiscAddr(  "C", 0, 0, 3, 1, 0, 2, "0.0" );
		T = new MiscAddr(  "T", 0, 0, 3, 0, 0, 0, "0" );
		D = new MiscAddr(  "D", 0, 2, 2, 0, 0, 0, "00" );
		F = new MiscAddr(  "", 0, 0, 5, 1, 0, 0, "0" );
		CT = new MiscAddr( " ", 5, 0, 5, 1, 0, 2, "0" );
		ORIENT = new MiscAddr( " ", 5, 0, 3, 0, 0, 0, "0" );


		UNITS  = new Group( null );
		ABSINC = new Group( null );
		COMP   = new Group( null );
		TOR    = new Group( null );
		MODE   = new Group( null );
		STRIP  = new Group( null );

		PRESS  = new Group( null );
		OFFSET = new Group( null );
		STOP   = new Group( null );
		PUNCH  = new Group( null );
		LOAD   = new Group( null );
		// PIERCE = new Group( null );
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
		
		G61 = new Symbol( MODE,   "G61",  0 );	// nibble mode?
		G64 = new Symbol( MODE,   "G64",  1 );	// punch mode?

		M17 = new Symbol( TOR,    "M17",  0 );	// TORCH ON
		M18 = new Symbol( TOR,    "M18",  1 );	// TORCH OFF DROP DOOR CLOSED
		M19 = new Symbol( TOR,    "M19", -1 );	// TORCH OFF DROP DOOR OPEN

		G90 = new Symbol( ABSINC, "G90",  0 );	// absolute
		G91 = new Symbol( ABSINC, "G91",  1 );	// incremental
		
		G177M54 = new Symbol( STRIP, "G177M54",  0 );	// extended stripper height
		G177M55 = new Symbol( STRIP, "G177M55",  1 );	// normal stripper height
		G177M56 = new Symbol( STRIP, "G177M56",  2 );	// tool change stripper height
		
		M00 = new Symbol( STOP,   "M00",  0 );	// hard stop
		M01 = new Symbol( STOP,   "M01",  1 );	// optional stop

		M40 = new Symbol( PRESS,  "M40 (MARKING PRESSURE)",  0 );
		M41 = new Symbol( PRESS,  "M41 (FORMING PRESSURE)",  1 );
		M42 = new Symbol( PRESS,  "M42 (PRESSURE OFF)",      2 );

		M85 = new Symbol( PRESS,  "M85",  0 );	// disable punching ram (disengage clutch)
		M75 = new Symbol( PRESS,  "M75",  1 );	// enable punching ram
		G70 = new Symbol( PRESS,  "G70",  2 );  // inhibit punch

		M87 = new Symbol( LOAD,   "M87(MANUAL LOAD)", 0 );
		M88 = new Symbol( LOAD,   "M88(AUTO LOAD)",   1 );
		M170 = new Symbol( LOAD,  "M170(AUTO LOAD)",  2 );  

		// M15 = new Symbol( PIERCE, "M15(PIERCE DELAY OFF)", 0 );
		// M65 = new Symbol( PIERCE, "M65(PIERCE DELAY ON)",  1 );
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
	boolean m_afterRepo	= false;
	boolean m_dropstop	= false;
	double m_cf			= 1.;		// Units conversion factor

	boolean	m_engaged	= false;
	double	m_xLoad		= 0.0;
	double	m_yLoad		= 0.0;

	int		m_tableVelocity		= 0;
	int		m_mode				= 0;

	int		m_maxHits	= 100;
	int		m_hits		= 0;

	DimAddr  X = null;
	DimAddr  Y = null;
	MiscAddr I = null;
	MiscAddr J = null;
	MiscAddr C = null;
	MiscAddr O = null;
	MiscAddr N = null;
	MiscAddr T = null;
	MiscAddr D = null;
	MiscAddr F = null;
	MiscAddr CT = null;
	MiscAddr ORIENT = null;
	DimAddr G175X = null;
	MiscAddr L = null;


	Group UNITS  = null;
	Group ABSINC = null;
	Group TOR    = null;
	Group COMP   = null;
	Group PRESS  = null;
	Group OFFSET = null;
	Group STOP   = null;
	Group STRIP  = null;
	Group PUNCH  = null;
	Group LOAD   = null;
	// Group PIERCE = null;
	Group MOTION = null;
	Group LINEAR = null;
	Group ARC    = null;
	Group MODE   = null;

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

	Symbol G61 = null;
	Symbol G64 = null;
	
	Symbol M17 = null;
	Symbol M18 = null;
	Symbol M19 = null;

	Symbol G90 = null;
	Symbol G91 = null;
	
	Symbol G177M54 = null;
	Symbol G177M55 = null;
	Symbol G177M56 = null;

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
	
	// NOTE: m_table is defined here only for convenience.
	//Vector<Double>	m_table;
	//Vector<String> 	m_table = new Vector<String>();
	double	m_feedrate;
	double	m_pierce_time;
	
	int		m_first_tnum;
	int		m_tool_count;
	boolean	did_repo;
}











