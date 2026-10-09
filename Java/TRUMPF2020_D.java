// cg.units|dcombo|Units|Inch|Metric|Inch to Metric|Metric to Inch|Inch
// cg.prog|dcombo|Programmer|GKK|GW|Other|GKK
// cg.die|str|Die Clerance|0.005
// cg.xload|dbl|Load X|14.955
// cg.yload|dbl|Load Y|5.512

import Weng.System.*;
import Weng.Modeler.*;
import Weng.Math.*;
import Weng.Geometry.*;
import Weng.CodeGen.*;
import Weng.Access.*;
import java.io.*;
import java.util.*;

public class TRUMPF2020_D implements WengMacro
{
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Frequently modified values.
	//
	private final boolean DEBUG = false;
	private final boolean TYPE = false;

	private double RAPID_RATE = 25000;		// ipm (previously a constant)
	private final double TOOL_CHANGE_TIME = 6.0 / 60.;
	private final double PUNCH_HIT_TIME = 0.42 / 60;
	private final double REPO_TIME = 10.0 / 60;

	boolean m_useSeqNum = true;
	int m_seqNum = 1;
	int m_seqNumInc = 2;

	public class ToolData
	{
		public int		toolNo;
		public int		toolType;
		public int		doff;
		public double	stationSize;
		public double	pitch;
		public String	toolDesc;
	}

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Main processing loop.
	//
	public void main()
	{
		int fileStatus;

		try
		{
			if (DEBUG) Msg.Display("Initializing");
			Initialize();
			if (DEBUG) Msg.Display("Finished initializing");

			fileStatus = output.Open();

			output.Open();


			while ( !clfile.AtEnd() )
			{
				clfile.Read();

				if (DEBUG && TYPE) output.Dump( "  >> record type: " + clfile.RecType() );

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
					m_xHold = clfile.Xe();
					m_yHold = clfile.Ye();
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

//			if (fileStatus == OutSys.FILE)
//				RenameFile();
		}
		catch(Exception e)
		{
			ExceptionPrinter.StackTracePrint(e);
		}

		Finalize();
	}

	private void Initialize()
	{
		clfile = new Clfile();
		output = new OutSys();

		ClassVarsInit();
		SymbolsInit();
		ClampsInit();

		G40.Uncond();

		M41.Uncond();

		PUNCH.Uncond(0);
	}

	private void Finalize()
	{
		clfile = null;
		output = null;
	}

	private void RenameFile()
	{
		File	srcFile;
		File	dstFile;
		String	srcName;
		String	dstName;
		String	ext;
		File	tmp;
		int		indx, jndx;

		srcName = output.Path();
		indx = srcName.lastIndexOf('\\');
		jndx = srcName.lastIndexOf('.');

		if (jndx < indx)
		{
			// No file extension found?
			dstName = srcName + ".CNC";
		}
		else
		{
			dstName = srcName.substring( 0, indx ) + ".CNC";
		}

		srcFile = new File( srcName );
		dstFile = new File( dstName );

		srcFile.renameTo( dstFile );
	}

	private void ToolReport()
	{
		int [] toolNums;
		int count, indx;
		int toolNo;

		toolNums = new int[20];
		count = 0;

		clfile.StatePush();

		while ( !clfile.AtEnd() )
		{
			clfile.Read();


			if (clfile.RecType() == Clfile.eToolChangeEnd)
			{
			    toolNo = clfile.Int("NC_Code_Number");

			    for (indx = 0; indx < count; ++ indx)
			    {
			    	if (toolNums[indx] == toolNo)
			    		break;
			    }

			    if (indx >= count)
			    {
			    	toolNums[count] = toolNo;
			    	++count;
			    }
			}
		}

		clfile.StatePop();
		repoLocation = 0;
		output.Dump( "ZA,DA," + count );
		int mycount = 1;
		for (indx = 0; indx < count; ++indx)
		{
			toolNo = toolNums[indx];
			output.Dump( "DA, '" + toolNo + "'," + mycount );
			++mycount;
		}
		seqtlcount = count;
	}

	private void PTTReport()
	{
		int indx;
		int toolNo;
		int type;

		m_tool_data = new ToolData[40];
		m_count = 0;

		clfile.StatePush();

		while ( !clfile.AtEnd() )
		{
			clfile.Read();


			if (clfile.RecType() == Clfile.eToolChangeEnd)
			{
			    toolNo = clfile.Int("NC_Code_Number");

			    for (indx = 0; indx < m_count; ++ indx)
			    {
			    	if (m_tool_data[indx].toolNo == toolNo)
			    		break;
			    }

			    if (indx >= m_count)
			    {
			    	m_tool_data[m_count] = new ToolData();

					m_tool_data[m_count].toolNo = toolNo;
					m_tool_data[m_count].toolType =  ToolType();
					m_tool_data[m_count].doff = clfile.Int("Doff");
					m_tool_data[m_count].pitch = clfile.Dbl("Pitch");
					m_tool_data[m_count].stationSize =  clfile.Dbl( "Reqd_Station_Size" );
					m_tool_data[m_count].toolDesc = clfile.Str( "Description" ).toUpperCase();

			    	++m_count;
			    }
			}
		}

		clfile.StatePop();

		output.Dump( "ZA,DA," + m_count );

		for (indx = 0; indx < m_count; ++indx)
		{
			type = m_tool_data[indx].toolType;
			if (type == 13)
			{
				type = 13;
				output.Dump( "DA,'PTT-" + (indx+1) + "',0,0,0,0,0.00," + m_tool_data[indx].doff +  ","+ m_tool_data[indx].stationSize + ",1,0.000," + type + ",0.00,0,0.000,'" + m_tool_data[indx].toolNo + "',");
				output.Dump( "*     '" + m_tool_data[indx].toolDesc + "',1,0.000,0.000,0.000,0.000 ");

			}
			else if (type == 9 && (m_tool_data[indx].doff > 0) && (m_tool_data[indx].doff < 40))
			{
				type = 13;
				output.Dump( "DA,'PTT-" + (indx+1) + "',0,0,0,0,5.000," + m_tool_data[indx].doff +  "," + m_tool_data[indx].stationSize + ",1,0.000," + type + ",0.00,0,0.000,'" + m_tool_data[indx].toolNo + "',");
				output.Dump( "*     '" + m_tool_data[indx].toolDesc + "',1,0.000,0.000,0.000,0.000 ");

			}
			else if (type == 5)
			{
				type = 5;

			 	output.Dump( "DA,'PTT-" + (indx+1) + "',0,0,0,0.000,0.000,0,1," + m_tool_data[indx].pitch + "," + type + ",0.00,0,0.00,'" + m_tool_data[indx].toolNo + "',");
				output.Dump( "*     '" + m_tool_data[indx].toolDesc + "',1,0.000,0.000,0.000,0.000 ");

			}
			else if (type == 27)
			{
				output.Dump( "DA,'PTT-" + (indx+1) + "',0,0,0,0,0.000,0.000,0,1,0.00," + type + "," + m_tool_data[indx].stationSize + ",0,0.000,'" + m_tool_data[indx].toolNo + "',");

				output.Dump( "*     '" + m_tool_data[indx].toolDesc + "',1,0.000,0.000,2.835,0.000 ");

			}
			else if (type == 20)
			{
				output.Dump( "DA,'PTT-" + (indx+1) + "',0,0,0,0,0.000,0.000,0,1,0.00," + type + "," + m_tool_data[indx].stationSize + ",0,0.000,'" + m_tool_data[indx].toolNo + "',");
				output.Dump( "*     '" + m_tool_data[indx].toolDesc + "',1,0.000,0.000,0.000,0.000 ");
			}
			else
			{

				output.Dump( "DA,'PTT-" + (indx+1) + "',0,0,0,0,0.000,0.000,0,1,0.00," + type + "," + m_tool_data[indx].stationSize + ",0,0.000,'" + m_tool_data[indx].toolNo + "',");
				output.Dump( "*     '" + m_tool_data[indx].toolDesc + "',1,0.000,0.000,0.000,0.000 ");
//					output.Dump( "we are here****" + type);

			}

		}
	}


	private String ToolDesc()
	{
		String toolDesc = "";
		String sval = clfile.Str( "Description" ).toUpperCase();
		if (sval.length() > 0)
			toolDesc = sval;
		return toolDesc;
	}

	private void StartProgram()
	{
		String material, filename;
		double dx, dy, dz;
		double density, weight;
		int prognum;

		Calendar calendar = Calendar.getInstance();
		int day = calendar.get(java.util.Calendar.DATE);
		int month = (calendar.get(java.util.Calendar.MONTH)+1);
		int year = calendar.get(java.util.Calendar.YEAR);

		ProgNum.LimitsSet( 1, 9999 );
		prognum = ProgNum.NextGet();

		filename = CNCFile();

		material = Model.StringGet("MatCfg");
		dx = Model.DoubleGet("Length");
		dy = Model.DoubleGet("Width") ;
		dz = Model.DoubleGet("Thickness");
		weight = Model.DoubleGet("Materialweight");
		prog = Model.StringGet( "cg.prog" );
		clamp1 = Model.IntGet("cg.clamp1") ;
		clamp2 = Model.IntGet("cg.clamp2") ;
		if(clamp1 > 100)			Msg.Display( "Assign clamps"  );

//		density = 0.283 / 2.204622; 4/29/02
//		weight = (dx * dy * dz) * density;


		if (DEBUG) output.Dump( "====>StartProgram()" );

		output.Dump( "BD" );
		output.Dump( "SET_INCH" );
		output.Dump( "BEGIN_EINRICHTEPLAN_INFO" );
		output.Dump( "ZA,MM,24");
		output.Dump( "MM,AT,1,  10,1,1,,'Machine'                          ,,'',T" );
		output.Dump( "MM,AT,1,  20,1,1,,'Type'                             ,,'',Z" );
		output.Dump( "MM,AT,1,  30,1,1,,'Control System'                   ,,'',T" );
		output.Dump( "MM,AT,1,  40,1,1,,'Variante'                         ,,'',Z" );
		output.Dump( "MM,AT,1,  50,1,1,,'Firm'                             ,,'',T" );
		output.Dump( "MM,AT,1,  60,1,1,,'Program Number'                   ,,'',Z" );
		output.Dump( "MM,AT,1,  70,1,1,,'Processor'       				   ,,'',T" );
		output.Dump( "MM,AT,1,  80,1,1,,'Date'                             ,,'',T" );
		output.Dump( "MM,AT,1,  90,1,1,,'Job Name'                         ,,'',T" );
		output.Dump( "MM,AT,1, 100,1,1,,'Number of Program Runs'           ,,'',Z" );
		output.Dump( "MM,AT,1, 110,1,1,,'Sheet Name'                       ,,'',T" );
		output.Dump( "MM,AT,1, 120,1,1,,'Memory Requirement'               ,,'',Z" );
		output.Dump( "MM,AT,1, 130,1,1,,'Material ID'                      ,,'',T" );
		output.Dump( "MM,AT,1, 140,1,1,,'Sheet Weight'                     ,,'lbs',Z" );
		output.Dump( "MM,AT,1, 150,1,1,,'Processing Time'                    ,,'min',Z" );
		output.Dump( "MM,AT,1, 160,1,1,,'Comment'                        ,,'',T" );
		output.Dump( "MM,AT,1, 170,1,1,,'Flag Automated'                   ,,'Bool',Z" );
		output.Dump( "MM,AT,1, 180,1,1,,'Flag ToPsxxx-Program'             ,,'Bool',Z" );
		output.Dump( "MM,AT,1, 190,1,1,,'Einrichteplanfilename'                ,,'',T" );
		output.Dump( "MM,AT,1, 200,1,1,,'Warehouse designation'                ,,'',T" );
		output.Dump( "MM,AT,1, 210,1,1,,'Palettierungsflag'                     ,,'Bool',Z" );
		output.Dump( "MM,AT,1, 220,1,1,,'Palettierungmodus'                     ,,'',Z" );
		output.Dump( "MM,AT,1, 250,1,1,,'SystemPalletType'                      ,,'',T" );
		output.Dump( "MM,AT,1, 280,1,1,,'viewer name machine'                   ,,'',T" );
		output.Dump( "ZA,DA,1" );
		output.Dump( "DA,'TC2020',2,'Bo Typ3',1,'Diamond','" + CNCFile() + "','"+ prog +"','" + month + "/" + day + "/" + year + "','',1," );
		output.Dump( "*  'LST'," + prognum + ",'" + material +" ', " + C.Uncond(weight) + ",5.55,'',1,1," );
		output.Dump( "*  'PDF','',0,1,'','TruPunch 2020 (S02)'" );
		output.Dump( "ENDE_EINRICHTEPLAN_INFO" );
		output.Dump( "BEGIN_PTT" );
		output.Dump( "ZA,MM,21" );
		output.Dump( "MM,AT,1,  10,1,1,,'Table Identifier'                    ,,'',T" );
		output.Dump( "MM,AT,1,  20,1,1,,'GEWIRO Number of Rotations'     ,,'1/min',Z" );
		output.Dump( "MM,AT,1,  30,1,1,,'GEWIRO Lubricant Container No.'      ,,'',Z" );
		output.Dump( "MM,AT,1,  40,1,1,,'Numbet Of Lub Impulses'              ,,'',Z" );
		output.Dump( "MM,AT,1,  50,1,1,,'Softpunch'                        ,,'Bool',Z" );
		output.Dump( "MM,AT,1,  60,1,1,,'UDP-Offset'                        ,,'in',Z" );
		output.Dump( "MM,AT,1,  70,1,1,,'LDP-Offset'                        ,,'in',Z" );
		output.Dump( "MM,AT,1,  80,1,1,,'Working Position'                    ,,'',Z" );
		output.Dump( "MM,AT,1,  90,1,1,,'TRUMPF-Identifier'                   ,,'',Z" );
		output.Dump( "MM,AT,1, 100,1,1,,'Number Of C Axis Rotations'     ,,'1/min',Z" );
		output.Dump( "MM,AT,1, 110,1,1,,'Table Type(Tool Type)'             ,,'',Z" );
		output.Dump( "MM,AT,1, 120,1,1,,'Feed'                           ,,'in/min',Z" );
		output.Dump( "MM,AT,1, 130,1,1,,'Stangendruck Abstreifer'          ,,'Bool',Z" );
		output.Dump( "MM,AT,1, 150,1,1,,'Freifahrweg'                      ,,'in',Z" );
		output.Dump( "MM,AT,1, 210,1,1,,'Tool ID Number'                   ,,'',T" );
		output.Dump( "MM,AT,1, 220,1,1,,'Tools Remark'                     ,,'',T" );
		output.Dump( "MM,AT,1, 230,1,1,,'Wzg-ID Validity'                 ,,'',Z" );
		output.Dump( "MM,AT,1, 240,1,1,,'UT2_Offset'                       ,,'in',Z" );
		output.Dump( "MM,AT,1, 250,1,1,,'OT2_Offset'                       ,,'in',Z" );
		output.Dump( "MM,AT,1, 260,1,1,,'Vorschub je Hub'                  ,,'in',Z" );
		output.Dump( "MM,AT,1, 270,1,1,,'Geschwindigkeit'                  ,,'in/min',Z" );
		output.Dump( "C" );

		PTTReport();

//		output.Dump( "DA,'PTT-1',0,0,0,0,0.0000,0.0000,0,1,0.00,1,0.00,0" );
		output.Dump( "C" );
		output.Dump( "ENDE_PTT" );

		output.Dump( "C" );
		output.Dump( "BEGIN_SHEET_TECH" );
		output.Dump( "C" );
		output.Dump( "ZA,MM,25" );
		output.Dump( "MM,AT,1,  10,1,1,,'Tabel Identifier'                  ,,'',T" );
		output.Dump( "MM,AT,1,  20,1,1,,'Sheet Dimension X'                      ,,'in',Z" );
		output.Dump( "MM,AT,1,  30,1,1,,'Sheet Dimension Y'                      ,,'in',Z" );
		output.Dump( "MM,AT,1,  40,1,1,,'Sheet Dimension Z'                      ,,'in',Z" );
		output.Dump( "MM,AT,1,  50,1,1,,'Sheet Type'                         ,,'',Z" );
		output.Dump( "MM,AT,1,  60,1,1,,'Sheet Thickness Offset'                ,,'in',Z" );
		output.Dump( "MM,AT,1,  70,1,1,,'Scraper Arm'                       ,,'Bool',Z" );
		output.Dump( "MM,AT,1,  80,1,1,,'Material-ID'                      ,,'',T" );
		output.Dump( "MM,AT,1,  90,1,1,,'Number Of Clamps'                   ,,'',Z" );
		output.Dump( "MM,AT,1, 100,1,1,,'Magazine Station Clamp 1'            ,,'',Z" );
		output.Dump( "MM,AT,1, 110,1,1,,'Magazine Station Clamp 2'            ,,'',Z" );
		output.Dump( "MM,AT,1, 120,1,1,,'Magazine Station Clamp 3'            ,,'',Z" );
		output.Dump( "MM,AT,1, 130,1,1,,'Magazine Station Clamp 4'            ,,'',Z" );
		output.Dump( "MM,AT,1, 160,1,1,,'Magazine Station Clamp 5'            ,,'',Z" );
		output.Dump( "MM,AT,1, 170,1,1,,'Magazine Station Clamp 6'            ,,'',Z" );
		output.Dump( "MM,AT,1, 180,1,1,,'1.Slot Covered By Sheet'             ,,'',Z" );
		output.Dump( "MM,AT,1, 190,1,1,,'Last Covered Sheet'               ,,'',Z" );
		output.Dump( "MM,AT,1, 200,1,1,,'TRUMPF-ID'                       ,,'',Z" );
		output.Dump( "MM,AT,1, 210,1,1,,'Machining Between Clamps'     ,,'',Z" );
		output.Dump( "MM,AT,1, 220,1,1,,'Sheet Dimension X real'                 ,,'mm',Z" );
		output.Dump( "MM,AT,1, 230,1,1,,'Sheet Dimension Y real'                 ,,'mm',Z" );
		output.Dump( "MM,AT,1, 240,1,1,,'Material ID'                  ,,'',T" );
		output.Dump( "MM,AT,1, 250,1,1,,'Acceleration Adjustment'          ,,'Bool',Z" );
		output.Dump( "MM,AT,1, 260,1,1,,'Material Density'                 ,,'lb/in3',Z" );
		output.Dump( "MM,AT,1, 290,1,1,,'Werkstoffkennung'                      ,,'',T" );
		output.Dump( "C" );
		output.Dump( "ZA,DA,1" );
		// length, width, thickness, clamps ....
		output.Dump( "DA,'SHT-1',"
					+ CT.Uncond(dx) + "," + CT.Uncond(dy) + "," + CT.Uncond(dz)
					+ ",1,0.00,0,0,2," + clamp1 + "," + clamp2 + ",0,0,0,0,1,20," );
		output.Dump( "*  1,-1," + CT.Uncond(dx) + "," + CT.Uncond(dy) + ",'" + material + "',1,0.29,'AlMg3'");
		output.Dump( "C" );
		output.Dump( "ENDE_SHEET_TECH" );
		output.Dump( "C" );

		output.Dump( "BEGIN_SHEET_LOAD" );
		output.Dump( "C" );
		output.Dump( "ZA,MM,21" );
		output.Dump( "MM,AT,1,         10,  1,1,,'TABLE IDENTIFIER'         ,,'',T" );
		output.Dump( "MM,AT,1,         20,  1,1,,'LOADING POSITION X'       ,,'in',Z" );
		output.Dump( "MM,AT,1,         30,  1,1,,'LOADING POSITION Y'       ,,'in',Z" );
		output.Dump( "MM,AT,1,         40,  1,1,,'FEED TO LOADING POSITION' ,,'in/min',Z" );
		output.Dump( "MM,AT,1,         50,  1,1,,'CALCULATION OF MULTITOOL OFFSET'  ,,'Bool',Z" );
		output.Dump( "MM,AT,1,         60,  1,1,,'TRIGGER FOR MATERIAL LOADING UB'  ,,'',Z" );
		output.Dump( "MM,AT,1,         70,  1,1,,'TRUMPF ID'                      ,,'',Z" );
		output.Dump( "MM,AT,1,        500,  1,1,,'LOADING UNIT'                   ,,'',Z" );
		output.Dump( "MM,AT,1,        510,  1,1,,'LIFT OFFSET X LOADING STACK'    ,,'in',Z" );
		output.Dump( "MM,AT,1,        520,  1,1,,'LIFT SUCTION CUP GROUP 1'       ,,'',Z" );
		output.Dump( "MM,AT,1,        530,  1,1,,'LIFT SUCTION CUP GROUP 2'       ,,'',Z" );
		output.Dump( "MM,AT,1,        540,  1,1,,'LIFT SUCTION CUP GROUP 3'       ,,'',Z" );
		output.Dump( "MM,AT,1,        550,  1,1,,'LIFT SUCTION CUP GROUP 4'       ,,'',Z" );
		output.Dump( "MM,AT,1,        560,  1,1,,'LIFT SUCTION CUP GROUP 5'       ,,'',Z" );
		output.Dump( "MM,AT,1,        570,  1,1,,'LIFT SWIVEL CUP'                ,,'',Z" );
		output.Dump( "MM,AT,1,        580,  1,1,,'DOUBLE SHEET DETECTOR ACTIVE'   ,,'Bool',Z" );
		output.Dump( "MM,AT,1,        590,  1,1,,'LEFT PEEL OFF'                  ,,'Bool',Z" );
		output.Dump( "MM,AT,1,        600,  1,1,,'LIFT SPEED'                    ,,'%',Z" );
		output.Dump( "MM,AT,1,        610,  1,1,,'LIFT ACCELERATION'             ,,'%',Z" );
		output.Dump( "MM,AT,1,        710,  1,1,,'INDEX PIN'                     ,,'',Z" );
		output.Dump( "MM,AT,1,        780,1,1,,'Einlegehilfe'                     ,,'Bool',Z" );



		output.Dump( "C" );
		output.Dump( "ZA,DA,1" );
		output.Dump( "DA,'SHL-1',"
					+ CT.Uncond(m_xLoad) + "," + CT.Uncond(m_yLoad) + ",4258.517,0,0,1,1,0.00,0,0,0,0,0,0," );
		output.Dump( "* 0,0,0.00,0.00,1,0" );
//		output.Dump( "DA,'SHL-1',80.118,5.512,4258.52,0,1,0.00,0,0,0,0,0,0,1," );
//		output.Dump( "*  0,0.0,0.0,0,1,1,1,0,1.0" );
		output.Dump( "C" );
		output.Dump( "ENDE_SHEET_LOAD" );
		output.Dump( "C" );
		output.Dump( "BEGIN_SHEET_UNLOAD" );
		output.Dump( "C" );
		output.Dump( "ZA,MM,70" );
		output.Dump( "MM,AT,1,           10,  1,1,,'TABLE IDENTIFIER'               ,,'',T" );
		output.Dump( "MM,AT,1,           20,  1,1,,'UNLOADING POSITION X'           ,,'in',Z" );
		output.Dump( "MM,AT,1,           30,  1,1,,'UNLOADING POSITION Y'           ,,'in',Z" );
		output.Dump( "MM,AT,1,           40,  1,1,,'FEED TO UNLOADING POSITION'     ,,'in/min',Z" );
		output.Dump( "MM,AT,1,           50,  1,1,,'MULTITOOL OFFSET'               ,,'Bool',Z" );
		output.Dump( "MM,AT,1,           60,  1,1,,'TRIGGER FOR MATERIAL UNLOADING UB'    ,,'',Z" );
		output.Dump( "MM,AT,1,           70,  1,1,,'TRUMPF ID'                            ,,'',Z" );
		output.Dump( "MM,AT,1,          200,  1,1,,'OUTSIDE GEOMETRY X'                   ,,'in',Z" );
		output.Dump( "MM,AT,1,          210,  1,1,,'OUTSIDE GEOMETRY Y'                   ,,'in',Z" );
		output.Dump( "MM,AT,1,          220,  1,1,,'OFFSET X'                             ,,'in',Z" );
		output.Dump( "MM,AT,1,          230,  1,1,,'RETRACT PUSH-OUT MOTION'              ,,'Bool',Z" );
		output.Dump( "MM,AT,1,          240,  1,1,,'PUSH-OUT MOTION X'        ,,'in',Z" );
		output.Dump( "MM,AT,1,          250,  1,1,,'PUSH-OUT MOTION Y'        ,,'in',Z" );
		output.Dump( "MM,AT,1,          260,  1,1,,'PUSH-OUT FEED RATE'       ,,'in/min',Z" );
		output.Dump( "MM,AT,1,          270,  1,1,,'WITH LIGHT BARRIER MONITORING'       ,,'Bool',Z" );
		output.Dump( "MM,AT,1,          280,  1,1,,'WITH LIGHT BARRIER SUPPORT'          ,,'Bool',Z" );
		output.Dump( "MM,AT,1,          290,  1,1,,'WITH DEFINED FLAP'                   ,,'',Z" );
		output.Dump( "MM,AT,1,          300,  1,1,,'FLAP OPEN(O)/CLOSED(1)'              ,,'',Z" );
		output.Dump( "MM,AT,1,          310,  1,1,,'MOVE TO HI POS AFTER CUTTING'          ,,'Bool',Z" );
		output.Dump( "MM,AT,1,          500,  1,1,,'ALL CLAMPS'                            ,,'Bool',Z" );
		output.Dump( "MM,AT,1,          510,  1,1,,'CLAMP 1'                               ,,'Bool',Z" );
		output.Dump( "MM,AT,1,          520,  1,1,,'CLAMP 2'                               ,,'Bool',Z" );
		output.Dump( "MM,AT,1,          530,  1,1,,'CLAMP 3'                               ,,'Bool',Z" );
		output.Dump( "MM,AT,1,          540,  1,1,,'CLAMP 4'                               ,,'Bool',Z" );
		output.Dump( "MM,AT,1,          550,  1,1,,'CLAMP 5'                               ,,'Bool',Z" );
		output.Dump( "MM,AT,1,          560,  1,1,,'CLAMP 6'                               ,,'Bool',Z" );
		output.Dump( "MM,AT,1,          570,  1,1,,'Bucket depositing site'                  ,,'',Z" );
		output.Dump( "MM,AT,1,          580,  1,1,,'Unloading device'                        ,,'',Z" );
		output.Dump( "MM,AT,1,          590,  1,1,,'LIFT_removal position'                   ,,'in',Z" );
		output.Dump( "MM,AT,1,          600,  1,1,,'LIFT_depositing site'                    ,,'',Z" );
		output.Dump( "MM,AT,1,          610,  1,1,,'LIFT_OFFSET_IN_X'                        ,,'in',Z" );
		output.Dump( "MM,AT,1,          620,  1,1,,'SORT_OFFSET_IN_Y'                        ,,'',Z" );
		output.Dump( "MM,AT,1,          630,  1,1,,'LIFT_store material thickness'           ,,'in',Z" );
		output.Dump( "MM,AT,1,          640,  1,1,,'LIFT_suction cup group 1'                ,,'',Z" );
		output.Dump( "MM,AT,1,          650,  1,1,,'LIFT_suction cup group 2'                ,,'',Z" );
		output.Dump( "MM,AT,1,          660,  1,1,,'LIFT_suction cup group 3'                ,,'',Z" );
		output.Dump( "MM,AT,1,          670,  1,1,,'LIFT_suction cup group 4'                ,,'',Z" );
		output.Dump( "MM,AT,1,          680,  1,1,,'LIFT_suction cup group 5'                ,,'',Z" );
		output.Dump( "MM,AT,1,          690,  1,1,,'LIFT_lift speed'                         ,,'%',Z" );
		output.Dump( "MM,AT,1,          700,  1,1,,'LIFT_lift acceleration'                  ,,'%',Z" );
		output.Dump( "MM,AT,1,          710,  1,1,,'LIFT_sort speed'                         ,,'%',Z" );
		output.Dump( "MM,AT,1,          720,  1,1,,'LIFT_sort acceleration'                  ,,'%',Z" );
		output.Dump( "MM,AT,1,          730,  1,1,,'Deposit part'                            ,,'Bool',Z" );
		output.Dump( "MM,AT,1,          740,  1,1,,'Remove part from clamp'                  ,,'Bool',Z" );
		output.Dump( "MM,AT,1,          750,  1,1,,'Drop part from top'                      ,,'Bool',Z" );
		output.Dump( "MM,AT,1,          760,  1,1,,'Drop part from defined height'           ,,'Bool',Z" );
		output.Dump( "MM,AT,1,          770,  1,1,,'Part under machine table'                ,,'Bool',Z" );
		output.Dump( "MM,AT,1,          820,  1,1,,'Go back after Ausschieben'               ,,'Bool',T" );
		output.Dump( "MM,AT,1, 			860,  1,1,,'minimale Flaeche'                 ,,'in2',Z" );
		output.Dump( "MM,AT,1, 			870,  1,1,,'maximale Flaeche'                 ,,'in2',Z" );
		output.Dump( "MM,AT,1,         1000,  1,1,,'Root table'                              ,,'',T" );
		output.Dump( "MM,AT,1,         1010,  1,1,,'Outer contour file'                      ,,'',T" );
		output.Dump( "MM,AT,1,         1020,  1,1,,'Stack item'                              ,,'',Z" );
		output.Dump( "MM,AT,1,         1030,  1,1,,'Drawing number'                          ,,'',T" );
		output.Dump( "MM,AT,1,         1040,  1,1,,'Preset deposition height'                ,,'in',Z" );
		output.Dump( "MM,AT,1,         1050,  1,1,,'Preset deposition type'                  ,,'',Z" );
		output.Dump( "MM,AT,1,         1060,  1,1,,'Preset disposal device'                  ,,'',Z" );
		output.Dump( "MM,AT,1,         1070,  1,1,,'Circumscribing rectangle X'              ,,'in',Z" );
		output.Dump( "MM,AT,1,         1080,  1,1,,'Circumscribing rectangle Y'              ,,'in',Z" );
		output.Dump( "MM,AT,1,         1090,  1,1,,'Preset stack thickness'                  ,,'in',Z" );
		output.Dump( "MM,AT,1,         1100,  1,1,,'Preset stack height'                     ,,'in',Z" );
		output.Dump( "MM,AT,1,         1110,  1,1,,'Preset measuring part fequency'          ,,'',Z" );
		output.Dump( "MM,AT,1,         1120,  1,1,,'Preset piece number per stack'           ,,'',Z" );
		output.Dump( "MM,AT,1,         1130,  1,1,,'Preset pallet offset X'                  ,,'in',Z" );
		output.Dump( "MM,AT,1,         1140,  1,1,,'Preset pallet offset Y'                  ,,'in',Z" );
		output.Dump( "MM,AT,1,         1150,  1,1,,'Preset disposal unit no.'                ,,'',Z" );
		output.Dump( "MM,AT,1,         1160,  1,1,,'Offset X suction frame part'             ,,'in',Z" );
		output.Dump( "MM,AT,1,         1170,  1,1,,'Offset Y suction frame part'             ,,'in',Z" );
		output.Dump( "MM,AT,1,         1180,  1,1,,'Drawing number'                          ,,'',T" );
		output.Dump( "MM,AT,1,         1190,  1,1,,'Drawing file name' 		                      ,,'',T" );
		output.Dump( "ZA,DA,1" );
		output.Dump( "DA,'SHU-1',38.932,3.723,4258.517.000,0,0,1,0.0,0.0,0.0,0,0.000,0.000,590.550,0," );
		output.Dump( "*  0,0,1,1,1,0,0,0,0,0,0,0,1,0.000,0,0.000,0.000,0.000,0,0,0,0,0,0.00," );
		output.Dump( "*  0.00,0.00,0.00,0,0,0,0,0,1,1.7,0.0,' ',' ',0,'',0,0,0,0,0,0,0,0,0,0,0," );
		output.Dump( "*  0,0.000,0.000,'',''" );
//		output.Dump( "DA,'SHU-1',28.694,50.000,4258.517,0,0,1,0.0,0.0,0.0,1," );
//		output.Dump( "*  0.000,0.000,0.000,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0.000,0," );
//		output.Dump( "*  0.000,0.000,0.000,0,0,0,0,0,0.00,0.00,50.00,50.00,0,0,0," );
//		output.Dump( "*  0,0,1,' ',' ',0,'',0,0,0,0,0,0,0,0,0,0,0,0,0,0,'',''	" );

		output.Dump( "C" );
		output.Dump( "ENDE_SHEET_UNLOAD" );
		output.Dump( "C" );
		output.Dump( " BEGIN_PART_UNLOAD" );
		output.Dump( "ZA,MM,66" );
		output.Dump( "MM,AT,1,  10,1,1,,'Tabellenidentifikator'            ,,'',T" );
		output.Dump( "MM,AT,1,  20,1,1,,'Huellgemetrie X'                  ,,'in',Z" );
		output.Dump( "MM,AT,1,  30,1,1,,'Huellgemetrie Y'                  ,,'in',Z" );
		output.Dump( "MM,AT,1,  40,1,1,,'Offset X'                         ,,'in',Z" );
		output.Dump( "MM,AT,1,  50,1,1,,'Ausschiebeweg X'                  ,,'in',Z" );
		output.Dump( "MM,AT,1,  60,1,1,,'Ausschiebeweg Y'                  ,,'in',Z" );
		output.Dump( "MM,AT,1,  70,1,1,,'Ausschiebevorschub'               ,,'in/min',Z" );
		output.Dump( "MM,AT,1,  80,1,1,,'mit Lichtschrankenueberwachung'   ,,'Bool',Z" );
		output.Dump( "MM,AT,1,  90,1,1,,'mit Blasluftunterstuetzung'       ,,'Bool',Z" );
		output.Dump( "MM,AT,1, 100,1,1,,'mit Ausdrueckzylinder'            ,,'Bool',Z" );
		output.Dump( "MM,AT,1, 110,1,1,,'mit definierter Klappe'           ,,'',Z" );
		output.Dump( "MM,AT,1, 120,1,1,,'Klappe auf (0)/ zu (1)'           ,,'',Z" );
		output.Dump( "MM,AT,1, 130,1,1,,'Handentsorgung'                   ,,'Bool',Z" );
		output.Dump( "MM,AT,1, 140,1,1,,'WST im Restgitter'                ,,'Bool',Z" );
		output.Dump( "MM,AT,1, 150,1,1,,'Trennen vor Oeffenen'             ,,'Bool',Z" );
		output.Dump( "MM,AT,1, 160,1,1,,'Senken vor Schwenken'             ,,'Bool',Z" );
		output.Dump( "MM,AT,1, 170,1,1,,'Ausschieben vor Oeffnen'          ,,'Bool',Z" );
		output.Dump( "MM,AT,1, 180,1,1,,'HOELA fahren nach Trennen'        ,,'Bool',Z" );
		output.Dump( "MM,AT,1, 200,1,1,,'Multitoolv. Zurueck nach Auss.'   ,,'Bool',Z" );
		output.Dump( "MM,AT,1, 210,1,1,,'TRUMPF-Kennung'                   ,,'',Z" );
		output.Dump( "MM,AT,1, 570,1,1,,'Kuebel Ablageort'                 ,,'',Z" );
		output.Dump( "MM,AT,1, 580,1,1,,'Entladegeraet'                    ,,'',Z" );
		output.Dump( "MM,AT,1, 590,1,1,,'LIFT_ENTNAHMEPOS'                 ,,'in',Z" );
		output.Dump( "MM,AT,1, 600,1,1,,'LIFT_ABLEGEORT'                   ,,'',Z" );
		output.Dump( "MM,AT,1, 610,1,1,,'LIFT_OFFSET_IN_X'                 ,,'in',Z" );
		output.Dump( "MM,AT,1, 620,1,1,,'SORT_OFFSET_IN_Y'                 ,,'mm',Z" );
		output.Dump( "MM,AT,1, 630,1,1,,'LIFT_MATERIALDICKE_ABLEGEN'       ,,'in',Z" );
		output.Dump( "MM,AT,1, 640,1,1,,'LIFT_SAUGERGRUPPE_1'              ,,'',Z" );
		output.Dump( "MM,AT,1, 650,1,1,,'LIFT_SAUGERGRUPPE_2'              ,,'',Z" );
		output.Dump( "MM,AT,1, 660,1,1,,'LIFT_SAUGERGRUPPE_3'              ,,'',Z" );
		output.Dump( "MM,AT,1, 670,1,1,,'LIFT_SAUGERGRUPPE_4'              ,,'',Z" );
		output.Dump( "MM,AT,1, 680,1,1,,'LIFT_SAUGERGRUPPE_5'              ,,'',Z" );
		output.Dump( "MM,AT,1, 690,1,1,,'LIFT_GESCHW_LIFT'                 ,,'%',Z" );
		output.Dump( "MM,AT,1, 700,1,1,,'LIFT_BESCHL_LIFT'                 ,,'%',Z" );
		output.Dump( "MM,AT,1, 710,1,1,,'LIFT_GESCHW_SORT'                 ,,'%',Z" );
		output.Dump( "MM,AT,1, 720,1,1,,'LIFT_BESCHL_SORT'                 ,,'%',Z" );
		output.Dump( "MM,AT,1, 740,1,1,,'LZYKL_T_UNTER_WZGAUFN_ENTN'       ,,'Bool',Z" );
		output.Dump( "MM,AT,1, 750,1,1,,'LZYKL_T_VON_OBEN_ABWERFEN'        ,,'Bool',Z" );
		output.Dump( "MM,AT,1, 760,1,1,,'LZYKL_T_AUS_H_BL_ST_VOLL_ABWRF'   ,,'Bool',Z" );
		output.Dump( "MM,AT,1, 770,1,1,,'LZYKL_T_AUSGESCHOBEN_ENTN'        ,,'Bool',Z" );
		output.Dump( "MM,AT,1, 780,1,1,,'LZYKL_T_ABLEGEN'                  ,,'Bool',Z" );
		output.Dump( "MM,AT,1, 790,1,1,,'LZYKL_T_UNTER_MASCHINENTISCH'     ,,'Bool',Z" );
		output.Dump( "MM,AT,1, 800,1,1,,'LZYKL_TRENNHUB_EIN_MIT_VAKUUM'    ,,'Bool',Z" );
		output.Dump( "MM,AT,1, 810,1,1,,'Spaeneklappe geschlossen'         ,,'Bool',Z" );
		output.Dump( "MM,AT,1, 820,1,1,,'Zurueckfahren nach Ausschieben'   ,,'Bool',Z" );
		output.Dump( "MM,AT,1, 830,1,1,,'Auswahl Handentnahmeart'          ,,'',Z" );
//		output.Dump( "MM,AT,1, 860,1,1,,'minimale Flaeche'                 ,,'in2',Z" );
//	   	output.Dump( "MM,AT,1, 870,1,1,,'maximale Flaeche'                 ,,'in2',Z" );
		output.Dump( "MM,AT,1,1000,1,1,,'Wurzeltabelle'                    ,,'',Z" );
		output.Dump( "MM,AT,1,1010,1,1,,'Aussenkonturdatei'                ,,'',Z" );
		output.Dump( "MM,AT,1,1020,1,1,,'Stapelgut'                        ,,'',Z" );
		output.Dump( "MM,AT,1,1030,1,1,,'Zeichnungsnummer'                 ,,'',T" );
		output.Dump( "MM,AT,1,1040,1,1,,'Vorg Ablegehoehe'                 ,,'in',Z" );
		output.Dump( "MM,AT,1,1050,1,1,,'Vorg Ablegeart'                   ,,'',Z" );
		output.Dump( "MM,AT,1,1060,1,1,,'Vorg Ensorgegeraet'               ,,'',Z" );
		output.Dump( "MM,AT,1,1070,1,1,,'umschreibendes Rechteck x'        ,,'in',Z" );
		output.Dump( "MM,AT,1,1080,1,1,,'umschreibendes Rechteck y'        ,,'in',Z" );
		output.Dump( "MM,AT,1,1090,1,1,,'Vorg Stapeldicke'                 ,,'in',Z" );
		output.Dump( "MM,AT,1,1100,1,1,,'Vorg Stapelhoehe max'             ,,'in',Z" );
		output.Dump( "MM,AT,1,1110,1,1,,'Vorg Messteilfrequenz'            ,,'',Z" );
		output.Dump( "MM,AT,1,1120,1,1,,'Vorg Stueckzahl pro Stapel'       ,,'',Z" );
		output.Dump( "MM,AT,1,1130,1,1,,'Vorg Palettenoffset x'            ,,'in',Z" );
		output.Dump( "MM,AT,1,1140,1,1,,'Vorg Palettenoffset y'            ,,'in',Z" );
		output.Dump( "MM,AT,1,1150,1,1,,'Vorg Entsorgeeinheit Nr'          ,,'',Z" );
		output.Dump( "MM,AT,1,1160,1,1,,'Offset x Saugerrahmen Teil'       ,,'in',Z" );
		output.Dump( "MM,AT,1,1170,1,1,,'Offset y Saugerrahmen Teil'       ,,'in',Z" );
		output.Dump( "MM,AT,1,1180,1,1,,'Zeichnungsname'                   ,,'',T" );
		output.Dump( "MM,AT,1,1190,1,1,,'ZeichnungsFileName'               ,,'',T" );


		output.Dump( "C" );
		output.Dump( "ZA, DA, 1" );
		output.Dump( "DA,'PAU-1',0.0,0.0,0.0,0.000,0.000,590.550,0,0,0,0,1,1,1,1,0,0,1,0,1,0,1," );
		output.Dump( "*  0.000,0,0.000,0.000,0.000,0,0,0,0,0,0.00,0.00,50.00,50.00,0,0,0,0,0,0," );
		output.Dump( "*  0,0,1,0,' ',' ',0,'',0,0,0,0,0,0,0,0,0,0,0,0,0,0,'',''	" );
//		output.Dump( "DA,'PAU-2',0.0,0.0,0.0,0.000,0.000,590.550,1,0,0,0,0,1,1,1,0,0,1,0,1,0,1," );
//		output.Dump( "*  0.000,0,0.000,0.000,0.000,0,0,0,0,0,0.00,0.00,50.00,50.00,0,0,0,0,0,0," );
//		output.Dump( "*  0,0,1,0,' ',' ',0,'',0,0,0,0,0,0,0,0,0,0,0,0,0,0,'',''	" );



		output.Dump( "ENDE_PART_UNLOAD" );
		output.Dump( "C" );
		output.Dump( "BEGIN_WZG_STAMM" );
		output.Dump( "C" );
		output.Dump( "ZA,MM,42" );
		output.Dump( "MM,AT,1,        10, 1,1,,'MACHINE'                 ,,'',T" );
		output.Dump( "MM,AT,1,        20, 1,1,,'ID NUMBER'               ,,'',T" );
		output.Dump( "MM,AT,1,        30, 1,1,,'DUPLO NUMBER'            ,,'',Z" );
		output.Dump( "MM,AT,1,        40, 1,1,,'PUNCH NUMBER'            ,,'',Z" );
		output.Dump( "MM,AT,1,        50, 1,1,,'MAGAZINE STATION'        ,,'',Z" );
		output.Dump( "MM,AT,1,        60, 1,1,,'TOOL LOCATION'           ,,'',Z" );
		output.Dump( "MM,AT,1,        70, 1,1,,'INDEX'                   ,,'',Z" );
		output.Dump( "MM,AT,1,        80, 1,1,,'TOOL TYPE'               ,,'',Z" );
		output.Dump( "MM,AT,1,        90, 1,1,,'WHISPERTOOL'             ,,'',Z" );
		output.Dump( "MM,AT,1,       100, 1,1,,'MULTITOOL'               ,,'',Z" );
		output.Dump( "MM,AT,1,       110, 1,1,,'BLOCKING ID'             ,,'',Z" );
		output.Dump( "MM,AT,1,       120, 1,1,,'PRIORITY'                ,,'',Z" );
		output.Dump( "MM,AT,1,       130, 1,1,,'COMMENT'                 ,,'',Z" );
		output.Dump( "MM,AT,1,       140, 1,1,,'GEWIRO ROTATION'    ,,'1/MIN',Z" );
		output.Dump( "MM,AT,1,       150, 1,1,,'GEWIRO RIGHT-LEFT'       ,,'',Z" );
		output.Dump( "MM,AT,1,       160, 1,1,,'ROTATION'                ,,'',Z" );
		output.Dump( "MM,AT,1,       170, 1,1,,'ASSEMBLY RELATIVE POSITION'     ,,'',Z" );
		output.Dump( "MM,AT,1,       180, 1,1,,'TOOL DIMENSION 1'               ,,'in',Z" );
		output.Dump( "MM,AT,1,       190, 1,1,,'TOOL DIMENSION 2'               ,,'in',Z" );
		output.Dump( "MM,AT,1,       200, 1,1,,'TOOL DIMENSION 3'               ,,'in',Z" );
		output.Dump( "MM,AT,1,       210, 1,1,,'TOOL DIMENSION 4'               ,,'in',Z" );
		output.Dump( "MM,AT,1,       220, 1,1,,'TOOL DIMENSION 5'               ,,'in',Z" );
		output.Dump( "MM,AT,1,       230, 1,1,,'TOOL DIMENSION 6'               ,,'in',Z" );
		output.Dump( "MM,AT,1,       240, 1,1,,'FEED RATE MIN'               ,,'in/MIN',Z" );
		output.Dump( "MM,AT,1,       250, 1,1,,'FEED RATE MAX'               ,,'in/MIN',Z" );
		output.Dump( "MM,AT,1,       260, 1,1,,'FEED RATE RES'               ,,'in/MIN',Z" );
		output.Dump( "MM,AT,1,       270, 1,1,,'STROKE RATE MIN'                  ,,'',Z" );
		output.Dump( "MM,AT,1,       280, 1,1,,'STROKE RATE MAX'                  ,,'',Z" );
		output.Dump( "MM,AT,1,       290, 1,1,,'STROKE RATE ACT'                  ,,'',Z" );
		output.Dump( "MM,AT,1,       300, 1,1,,'DRAWING NAME'                     ,,'',T" );
		output.Dump( "MM,AT,1,       310, 1,1,,'PRESSER FOOT TYPE'                ,,'',Z" );
		output.Dump( "MM,AT,1,       320, 1,1,,'UDP POSITION, SHEET THICKNESS 0.0'  ,,'in',Z" );
		output.Dump( "MM,AT,1,       330, 1,1,,'TRUMPF ID'                     ,,'',Z" );
		output.Dump( "MM,AT,1,       340, 1,1,,'SHEET THICKNESS'               ,,'in',Z" );
		output.Dump( "MM,AT,1,       350, 1,1,,'SHEET THICKNESS TOLERANCE'     ,,'in',Z" );
		output.Dump( "MM,AT,1,       360, 1,1,,'REGRIND LENGTH'                ,,'in',Z" );
		output.Dump( "MM,AT,1,       370, 1,1,,'LONG TOOL'                     ,,'in',Z" );
		output.Dump( "MM,AT,1,       380, 1,1,,'MT ZERO POINT OFFSET X'        ,,'in',Z" );
		output.Dump( "MM,AT,1,       390, 1,1,,'MT ZERO POINT OFFSET Y'        ,,'in',Z" );
		output.Dump( "MM,AT,1, 		490,1,1,,'Hardware Qualification'                ,,'',Z" );
		output.Dump( "MM,AT,1, 		510,1,1,,'Data Source'                           ,,'',Z" );
		output.Dump( "MM,AT,1, 		520,1,1,,'Customer Remark1'                      ,,'',T" );
		output.Dump( "C	" );

		ToolPlan();
		output.Dump( "ENDE_WZG_STAMM");
		output.Dump( "C" );

		output.Dump( "BEGIN_SHEET_REPOSIT" );
		output.Dump( "C" );
		output.Dump( "ZA,MM,8" );
		output.Dump( "MM,AT,1,  10,1,1,,'Tabellenidentifikator'            ,,'',T" );
		output.Dump( "MM,AT,1,  20,1,1,,'Nachsetzweg X'                    ,,'in',Z" );
		output.Dump( "MM,AT,1,  30,1,1,,'Nachsetzweg Y'                    ,,'in',Z" );
		output.Dump( "MM,AT,1,  40,1,1,,'Nachsetzvorschub'                 ,,'in/min',Z" );
		output.Dump( "MM,AT,1,  50,1,1,,'Verrechnung Multitoolversatz'     ,,'Bool',Z" );
		output.Dump( "MM,AT,1,  60,1,1,,'Nachsetzart'                      ,,'',Z" );
		output.Dump( "MM,AT,1,  70,1,1,,'TRUMPF-Kennung'                   ,,'',Z" );
		output.Dump( "MM,AT,1, 100,1,1,,'Stanzkopflage Offset'             ,,'mm',Z" );
		output.Dump( "C" );

		RPOTable();

		output.Dump( "C" );
		output.Dump( "ENDE_SHEET_REPOSIT" );
		output.Dump( "C" );
		output.Dump( "BEGIN_WZG_CALLS" );
		output.Dump( "C" );
		output.Dump( "ZA,MM,2" );
		output.Dump( "MM,AT,1,  10,1,1,,'Tool ID Number'              ,,'',T" );
		output.Dump( "MM,AT,1,  20,1,1,,'Tool ID Number'             ,,'',Z" );
		output.Dump( "C" );

		ToolReport();

		output.Dump( "C" );
		output.Dump( "ENDE_WZG_CALLS" );
		output.Dump( "C" );
		output.Dump( "BEGIN_PROGRAMM" );
		output.Dump( "C" );
		output.Dump( "ZA,MM,4" );
		output.Dump( "MM,AT,1,  10,1,1,,'Program Nummer'                   ,,'',T" );
		output.Dump( "MM,AT,1,  20,1,1,,'Program Type'                      ,,'',T" );
		output.Dump( "MM,AT,1,  30,1,1,,'Comments'                        ,,'',T" );
		output.Dump( "MM,AT,1,  40,1,1,,'Machine Time'                 ,,'min',Z" );
		output.Dump( "C" );
		output.Dump( "ZA,DA,1" );
		output.Dump( "DA,'" + filename + "','HP','OTTENWELLER 1188668'," );
		output.Dump( "START_TEXT" );

		CondOutput( " MSG( \"MAIN PROGRAMME NUMBER," + filename + "\" )" );

		CondOutput( " MSG( \"DIMENSION OF SHEET: "
					+ CT.Uncond(dz) + " X " + CT.Uncond(dx) + " X " + CT.Uncond(dy)
					+ " MATERIAL ID: -0.0000\" )" );

		CondOutput( " TRAILON(C2,C1)" );
		CondOutput( " G70" );
		CondOutput( " G01 F4259" );
//		CondOutput( " PRESSERFOOT_OFF" );
		CondOutput( " TC_SUCTION_OFF" );


		CondOutput( " TC_CLAMP_CYC" );
 		CondOutput( " TC_TOOL_LUBE_ON(2)");
		CondOutput( " MSG( \"ZERO POINT 1.213 0\" )" );
		CondOutput( " MSG( \"LOAD SHEET\" )" );
		CondOutput( " TC_SHEET_TECH(\"SHT-1\")" );
		CondOutput( ";TC_TECHNO_MODE(1)" );
		CondOutput( ";GOTOF ENTRY_PUNCH" );
		CondOutput( " TC_SHEET_LOAD(\"SHL-1\")" );
		CondOutput( ";TC_TANGTOOL_OFF" );
 //		CondOutput( ";ENTRY_PUNCH:" );
		newrepox     = 0;
		newrepoy     = 0;

	}



	private void ToolPlan()
	{
		int [] toolNums;
		int count, indx;
		int toolNo;

		toolNums = new int[200];
		count = 0;

		clfile.StatePush();

		while ( !clfile.AtEnd() )
		{
			clfile.Read();


			if (clfile.RecType() == Clfile.eToolChangeEnd)
			{
			    toolNo = clfile.Int("NC_Code_Number");


			    for (indx = 0; indx < count; ++ indx)
			    {
			    	if (toolNums[indx] == toolNo)
			    		break;
			    }

			    if (indx >= count)
			    {
			    	toolNums[count] = toolNo;
			    	++count;

			    }
			}
		}

		clfile.StatePop();
		output.Dump( "ZA,DA," + count );

		count = 0;

		clfile.StatePush();

		while ( !clfile.AtEnd() )
		{
			clfile.Read();


			if (clfile.RecType() == Clfile.eToolChangeEnd)
			{
			    toolNo = clfile.Int("NC_Code_Number");


			    for (indx = 0; indx < count; ++ indx)
			    {
			    	if (toolNums[indx] == toolNo)
			    		break;
			    }

			    if (indx >= count)
			    {
			    	toolNums[count] = toolNo;
			    	++count;
					String toolDesc = clfile.Str( "Description" ).toUpperCase();
					double Dia = clfile.Dbl("Diameter" );
					int type = clfile.Int( "Type_ID" );
					double Wid = clfile.Dbl("Width");
					int doff = clfile.Int("Doff");
					double Len = clfile.Dbl("Length");
					output.Dump( "DA,\",'" + toolNo + "',0,1,1,0,0," + ToolType() + ",0,1,0,0,'" + toolDesc  + "',0.00,0,0,");
					if(type == Const.ROUND)
					{
						output.Dump( " *  0.0000," + Dia + ",0,0.0000,0.0000,0.0000,0.0000,0.0000,");
						output.Dump( " *  0.0000,0.0000,0.0,0.0,0.0,'rund.wzg',0,0.0000,1,0,0,0,0,0.0,0.0,0,0,'' ");
					}
					else if (type == Const.SQUARE)
					{
						output.Dump( " *  0.0000," + Wid + "," + Wid + ",0.0000,0.0000,0.0000,0.0000,0.0000,");
						output.Dump( " *  0.0000,0.0000,0.0,0.0,0.0,'sqr.wzg',0,0.0000,1,0,0,0,0,0.0,0.0,0,0,'' ");
					}
					else if (type == Const.FORMING && doff != 27)
					{
						output.Dump( " *  0.0000," + Wid + ",0,0.0000,0.0000,0.0000,0.0000,0.0000,");
						output.Dump( " *  0.0000,0.0000,0.0,0.0,0.0,'form.wzg',0,0.0000,1,0,0,0,0,0.0,0.0,0,0,'' ");
					}
					else if (type == Const.FORMING && doff == 27)
					{
						output.Dump( " *  20.0," + Len  + "," + Wid + ",0.0000,0.0000,0.0000,0.0000,0.0000,");
						output.Dump( " *  0.0000,0.0000,0.0,0.0,0.0,'form.wzg',0,0.0000,1,0,0,0,0,0.0,0.0,0,0,'' ");
					}
					else
					{
						output.Dump( " *  0.0000," + Len  + "," + Wid + ",0.0000,0.0000,0.0000,0.0000,0.0000,");
						output.Dump( " *  0.0000,0.0000,0.0,0.0,0.0,'rec.wzg',0,0.0000,1,0,0,0,0,0.0,0.0,0,0,'' ");
					}
			    }
			}
		}

		clfile.StatePop();


	}

	private int ToolType()
	{
		int type = clfile.Int( "Type_ID" );
		int doff = clfile.Int("Doff");

		if (type == Const.CENTER_PUNCH)			type =  10;
		else if (type == Const.ROUND)			type =  1;
		else if (type == Const.SQUARE)			type =  3;
		else if (type == Const.RECTANGLE)		type =  4;
		else if (type == Const.OBROUND)			type =  8;
		else if (type == Const.DIAMOND)			type =  7;
		else if (type == Const.CORNER_RADIUS)	type =  7;
		else if (type == Const.SINGLE_D)		type =  7;
		else if (type == Const.DOUBLE_D)		type =  7;
		else if (type == Const.TRAPEZOID)		type =  7;
		else if (type == Const.KEYHOLE)			type =  7;
		else if (type == Const.FORMING)			type =  13;
		else if (type == Const.MARKING)			type =  10;
		else if (type == Const.HEXAGON)			type =  7;
		else if (type == Const.CUSTOM)			type =  9;
		else if (type == Const.TAP)				type =  5;
		else type = 0;;
		if(clfile.Int("NC_Code_Number") == 7620500)
		{
			type = 27;
		}
		if(clfile.Int("NC_Code_Number") == 328998)
		{
			type = 19;
		}
		return type;
	}



	private void EndProgram()
	{
		String unloadPosition = ((m_metric)
								 ? X.AbsUncond(3048.0) + Y.AbsUncond(-1612.9)
								 : X.AbsUncond(120.0) + Y.AbsUncond(-63.5));

		int type = clfile.Int( "Type_ID" );
		int doff = clfile.Int("Doff");
		int toolType = m_currTool.IntGet("Type_ID");

		if (DEBUG) output.Dump( "====>EndProgram()" );
		CondOutput(NIBBLE.Cond(0));
		CondOutput(TAP.Cond(0));
		CondOutput(PUNCH.Cond(0));
		CondOutput(PUNCH.Cond(0));
		CondOutput(BEAD.Cond(0));
		CondOutput(MARK.Cond(0));
		if(doff == 27)
		{
			CondOutput(MULTISHEAR.Cond(0));
		}
		//CondOutput( " PRESSERFOOT_OFF" );
		//CondOutput( " TC_TECHNO_MODE(1)" );
		CondOutput( " TC_SUCTION_OFF" );
//		if (rpo > 0)
//		{
//			CondOutput( " TRANS X0.0 Y0.0");
//		}
		m_time += l_time;
		//CondOutput( " GOTOB STARTUP" );
		CondOutput( " TC_SHEET_UNLOAD(\"SHU-1\")" );
		CondOutput( " M30" );
		output.Dump( "STOP_TEXT" );
		output.Dump( "ENDE_PROGRAMM" );
		output.Dump( "ED" );
		Msg.Display( "----------------Diamond------------------Cycle time: " + CT.Uncond(m_time)  );
if(seqtool != seqtlcount)		Msg.Display( "STOP---------------------------------------------------------------------------------------------------------------------------: " + seqtool   + " tool count; " + seqtlcount);

Model.StringSet("Customer", CT.Uncond(m_time) );
	}

	private void ToolChange()
	{
		DbTool dbTool;
		++seqtool;
		int indx;
		int nextToolType;
		int nextToolNo, toolNo;
		int	doff= clfile.Int("Doff");
		m_engaged = false;//3/31/2008
		dbTool = clfile.Entity().Tool();
		toolNo = clfile.Int("NC_Code_Number");
		m_time += TOOL_CHANGE_TIME;
		nextToolNo = dbTool.IntGet("NC_Code_Number");
		DbEntity dbEntity = clfile.Entity( );



		CondOutput(NIBBLE.Cond(0));
		CondOutput(TAP.Cond(0));
		CondOutput(PUNCH.Cond(0));
		CondOutput(PUNCH.Cond(0));
		CondOutput(BEAD.Cond(0));
		CondOutput(MARK.Cond(0));

		if(sett == 1)
		{
			CondOutput(MULTISHEAR.Cond(0));
			sett = 0;
		}
			if(dcount == 1)
			{
//				CondOutput( " we are here -------------------" + DropMAN );
				CondOutput(" TC_PART_UNLOAD(\"PAU-1\")");
				 dcount = 0;
			}
		boolean isPressureFoot = (dbEntity.IntGet("PressureFoot") == 1);
		X.Set(48);
		Y.Set(0);
		C.Set(500);


		if (DEBUG) output.Dump( "====>ToolChange()");


		if ( IsPunchTool( ))
		{

			CondOutput( ";ENTRY_PUNCH:");
			CondOutput( " MSG(\"" + nextToolNo + "\")");
		if(lftool == 1)	CondOutput( " TC_TOOL_POSITION(CHANGE)" );
			CondOutput( " TC_TOOL_NO (\"" + nextToolNo + "\")" );
			CondOutput( " TC_TOOL_CHANGE" );

			if ( isPressureFoot == true)
			{
				CondOutput( " PRESSERFOOT_ON" );
			}
			else
			{
				CondOutput( " PRESSERFOOT_OFF" );
			}
			isPressureFoot = false;

			CondOutput( " TC_SUCTION_ON" );
			for (indx = 0; indx < m_count; ++indx)
			{
				if (nextToolNo == m_tool_data[indx].toolNo)
				{
					CondOutput( " TC_TOOL_TECH(\"PTT-" + (indx+1) + "\")" );
					break;
				}
			}
			lftool = 0;
			LastTool = nextToolNo;

		}
		else if ( IsFormingTool( ) )
		{

			CondOutput( ";ENTRY_PUNCH: ;" + nextToolNo );
			CondOutput( " TC_TANGTOOL_OFF" );
			CondOutput( " TC_TOOL_POSITION(CHANGE)" );
			CondOutput( " TC_TOOL_NO (\"" + nextToolNo + "\")" );
			CondOutput( " TC_TOOL_CHANGE" );
			CondOutput( " PRESSERFOOT_OFF" );

			for (indx = 0; indx < m_count; ++indx)
			{
				if (nextToolNo == m_tool_data[indx].toolNo)
				{
					if(nextToolNo == 7620500)
					{
//						CondOutput( " TC_SHEET_TECH(\"SHT-2\")" );
						CondOutput( " TC_SUCTION_ON" );
						multiS = 1;
						if(doff != 27)
						{
							Msg.Display( "DOFF IS NOT SET"  );
						}
					}
					else
					{
						CondOutput( " TC_SUCTION_OFF" );
					}
					CondOutput( " TC_TOOL_TECH(\"PTT-" + (indx+1) + "\")" );
					msptt = indx + 1;
					break;
				}
			}

			if(doff == 19)
			{
				passPTT = indx + 1;
//				CondOutput( BEAD.Cond(1) );
			}
			else
			{
				if(nextToolNo != 7620500)
				{

					CondOutput( PUNCH.Cond(1) );

				}
			}
			lftool = 1;
		}
		else if ( IsMarkingTool( ) )
		{

			CondOutput( ";ENTRY_PUNCH: ;" + nextToolNo );
			CondOutput( " TC_TANGTOOL_OFF" );
			CondOutput( " TC_TOOL_NO (\"" + nextToolNo + "\")" );
			CondOutput( " TC_TOOL_CHANGE" );
			CondOutput( " PRESSERFOOT_OFF" );
			CondOutput( " TC_SUCTION_OFF" );
			for (indx = 0; indx < m_count; ++indx)
			{
				if (nextToolNo == m_tool_data[indx].toolNo)
				{
					CondOutput( " TC_TOOL_TECH(\"PTT-" + (indx+1) + "\")" );
					break;
				}
			}


		}

		else if ( IsTapTool( ) )
		{

			CondOutput( ";ENTRY_PUNCH: ;" + nextToolNo );
			CondOutput( " TC_TANGTOOL_OFF" );
			CondOutput( " TC_TOOL_NO (\"" + nextToolNo + "\")" );
			CondOutput( " TC_TOOL_CHANGE" );
//			CondOutput( " TC_SOFTPUNCH_OFF" );
			CondOutput( " PRESSERFOOT_OFF" );
			CondOutput( " TC_SUCTION_OFF" );
			for (indx = 0; indx < m_count; ++indx)
			{
				if (nextToolNo == m_tool_data[indx].toolNo)
				{
					CondOutput( " TC_TOOL_TECH(\"PTT-" + (indx+1) + "\")" );
					break;
				}
			}
			 CondOutput(TAP.Cond(1));

		}

		m_currTool = dbTool;
		NewAngle = 360.0;	// cause "C1=DC()" to output when needed
		GRESET.Uncond();			// cause the next G-code to output when needed
//		Msg.Diagnostic( "Tool change Punch: " + m_time );
//	Msg.Display( "STOP-------: " + seqtool   + " tool count; " + seqtlcount);

	}

	private String Rapid( double xe, double ye, boolean force )
	{
		String block = "";

		m_engaged = false;

		if ( X.Delta( xe ) || Y.Delta( ye ) )
		{
			if ( m_incr )
				block = G00.Uncond() + X.IncrUncond( xe ) + Y.IncrUncond( ye );
			else
				block = G00.Uncond() + X.AbsUncond( xe ) + Y.AbsUncond( ye );

//			Msg.Diagnostic( "Current time: " + m_time );

//			m_time += (Math.sqrt( dx*dx + dy*dy ) / RAPID_RATE);
		}

		return block;
	}

	private void Line()
	{
		DbTool dbTool;
		DbEntity dbEntity;
		DbCurve	dbCurve;
		dbEntity = clfile.Entity();
		dbCurve = DbCurve.DbCurve( dbEntity );

		dbTool = clfile.Entity().Tool();
		double xe = clfile.Xe() * m_cf + newrepox;
		double ye = clfile.Ye() * m_cf + newrepoy;

		Line line = clfile.Line( );
		isHole = false;
		ClampSafe(dbEntity);
//		if ( !m_engaged )
		Engage();

		if ( X.Delta( xe ) || Y.Delta( ye ) )
		{
			String block;

			if (DEBUG) output.Dump( "====>Line()" );

			if ( m_incr )
				block = G01.Cond() + X.IncrCond( xe ) + Y.IncrCond( ye );
			else
				block = G01.Cond() + X.AbsCond( xe ) + Y.AbsCond( ye );

			if (COMP.Symbol() != G40 && clfile.Entity().IsLeadOut())
				block += (G40.Uncond() + D.Uncond(0));

			CondOutput( block );
		}
//		Drop();

		if ( IsPunchTool( ) )
		{
			m_hits = (line.ArcLen() / Feedrate());
			if(m_hits < 2) m_hits = 2;

//			Msg.Diagnostic( "Current time Punch: " + m_time );
		}

		m_hits = (line.ArcLen() / Feedrate());
		if(m_hits < 2) m_hits = 2;
		m_time += (m_hits * PUNCH_HIT_TIME);
		int nextToolNo = dbTool.IntGet("NC_Code_Number");
		if(nextToolNo == 7620500)
		{
			m_hits = (line.ArcLen() / 1.0);
			if(m_hits < 2) m_hits = 2;
			m_time += (m_hits * PUNCH_HIT_TIME);
		}
	}

	private void Arc()
	{
		double xs = clfile.Xs() * m_cf + newrepox;
		double ys = clfile.Ys() * m_cf + newrepoy;
		double xe = clfile.Xe() * m_cf + newrepox;
		double ye = clfile.Ye() * m_cf + newrepoy;
		double xc = clfile.Xc() * m_cf + newrepox;
		double yc = clfile.Yc() * m_cf + newrepoy;


		Arc arc = clfile.Arc();
		isHole = false;

		if ( !m_engaged )
			Engage();

		if (DEBUG) output.Dump( "====>Arc()" );

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
		Drop();

		if ( IsPunchTool( ) )
		{

			m_hits = (arc.ArcLen() / Feedrate());

		}


	}

	private void Feature()
	{
	}

	private void FeatureEnd()
	{
	}
	private void ClampCheck( double xe )
	{
		String fname;
		int indx;
		int myzone = 1;
double delta = 0 - repoLocation;
//		Msg.Display( " repolocation " + repoLocation);
		double myxe = clfile.Xe();
		double mycl1max = -3.2682 + (clamp1 * 5.1181) + delta ;
		double mycl1min = -10.906 + (clamp1 * 5.1181) + delta ;
		double mycl2max = -3.2682 + (clamp2 * 5.1181) + delta ;
		double mycl2min = -10.906 + (clamp2 * 5.1181) + delta ;

		if(repoLocation != 0 )
		{
			myzone = 2;
		}
		if(xe < mycl1max & xe > mycl1min)
		{
			Msg.Display( " Punch under clamp - 1 - " + xe + "-or impossible move Work Zone:" + myzone  );

		}
		if(xe < mycl2max & xe > mycl2min)
		{
			Msg.Display( " Punch under clamp - 2 - " + xe + "-or impossible move Work Zone:" + myzone  );
		}
	}
	private void ClampSafe( DbEntity dbEntity )
	{

		DbCurve	dbCurve;
		dbCurve = DbCurve.DbCurve( dbEntity );
		Point LinePS = dbCurve.StartPt();
		Point LinePE = dbCurve.EndPt();
		double delta = 0 - repoLocation;
		int myzone = 1;
		double clamp =0.0;
//		clamp = m_xLoad;
		boolean coll1 = false;
		boolean coll2 = false;

		m_clamp[1]= new SafeZone();


m_clamp[1].Set(
				-7.0871 + (clamp1 * 5.1181) + delta + clamp, 0,
				(-10.906 + (clamp1 * 5.1181) + delta + clamp),
				(0),  // - width),
				(-3.2682 + (clamp1 * 5.1181) + delta + clamp),
				(2.8346) );  // - width) );
	 coll1 = m_clamp[1].Collision(LinePS, LinePE, 0.0);

m_clamp[1].Set(
				-7.0871 + (clamp2 * 5.1181) + delta + clamp, 0,
				(-10.906 + (clamp2 * 5.1181) + delta + clamp),
				(0),  // - width),
				(-3.2682 + (clamp2 * 5.1181) + delta + clamp),
				(2.8346) );  // - width) );
	 coll2 = m_clamp[1].Collision(LinePS, LinePE, 0.0);


//Msg.Display( " RepoLocation "  + (-7.0871 + (clamp1 * 5.1181) + delta + clamp));
		double mycl1max = -3.2682 + (clamp1 * 5.1181) + delta ;
		double mycl1min = -10.906 + (clamp1 * 5.1181) + delta ;
		double mycl2max = -3.2682 + (clamp2 * 5.1181) + delta ;
		double mycl2min = -10.906 + (clamp2 * 5.1181) + delta ;


		if(repoLocation != 0)
		{
			myzone = 2;
		}
		if(coll1)
		{
			Msg.Display( " Punch under clamp - 1 - or impossible move X location:" );
		}
		if(coll2)
		{
			Msg.Display( " Punch under clamp - 2 - or impossible move X location:" );
		}
		double xs = LinePS.X() - delta;
		double xe = LinePE.X() - delta;
//		double xe =	clfile.Xe() - delta;
//Msg.Display( " RepoLocation " + xe + " ------"  + (-7.0871 + (clamp1 * 5.1181) + delta + clamp));
		if(xe < -.5 | xe > 100)
		{
			Msg.Display( " Punch - 1 - " + xe + "impossible move Work Zone:" + myzone  );
		}
		if(xs < -.5 | xs > 100)
		{
			Msg.Display( " Punch - 1 - " + xs + "impossible move Work Zone:" + myzone  );
		}
	}
	private void Hole()
	{
		if (DEBUG) output.Dump( "====>Hole()" );
		double xe = clfile.Xe() * m_cf + newrepox;
		double ye = clfile.Ye() * m_cf + newrepoy;
		int toolNo = clfile.Int("NC_Code_Number");
		if(toolNo == 7620500)
		{
			Msg.Display( " WE can not punch holes with the Multi Shear Tool");
			output.Dump( "STOP------------" );
		}
		if(ye < 2.8346)
		{
			double myxe = clfile.Xe();
			ClampCheck(myxe);
		}
		if(ye < -0.275)
		{
			Msg.Display( " WE can not punch holes at this location");
		}
		if(isHole != true)
		{
			m_engaged = false;
		}
		double myxe = xe;
		//CondOutput( "-----repoloc " + repoLocation + " myrepo "  +rpo);
		if(myxe > 100 || myxe < -.5)
		{
			Msg.Display( " Program needs a Reposition move: " + " Y:" + clfile.Int("NC_Code_Number") + " X:" + xe + " Repo =" + rpo);
		}

		boolean DropMAN = false;
		boolean Slow = false;
		DbEntity dbEntity = clfile.Entity( );
		DropMAN = (dbEntity.IntGet("DropMAN") == 1);
		Slow = (dbEntity.IntGet("Slow") == 1);
		if(DropMAN != true) DropMAN = (dbEntity.IntGet("DropMAN") == 2);
		if(Slow != true) Slow = (dbEntity.IntGet("Slow") == 2);
		if(DropMAN)
		{
			m_engaged = false;

		}
		if(Slow)
		{
			CondOutput( "G01 F100");
		}
		isPressureFoot = (dbEntity.IntGet("PressureFoot") == 1);
		isHole = true;
//		if ( !m_engaged )
//			m_engaged = false;/// changed 3/28/2008
		Engage();

		if ( X.Delta( xe ) || Y.Delta( ye ) )
		{
			String block;
			String ToolRot;
			int type = clfile.Int( "Type_ID" );
			double myangle = 0;

			if(IndexAngle() == NewAngle || type == 7)
			{
				ToolRot ="";
			}
			else
			{
			    toolNo = clfile.Int("NC_Code_Number");

				ToolRot = " C1=DC(" + CT.Uncond(IndexAngle()) + ")";
				NewAngle = IndexAngle();
			}

//			CondOutput( "====>Hole()" + NewAngle + " tool " + IndexAngle() );

			Point ps = new Point( X.Curr(), Y.Curr(), 0. );
			Point pe = new Point( xe, ye, 0. );

			if (DEBUG) output.Dump( "====>Hole()" );

			double degrees = clfile.Dbl("orient");

			if (rpo >0)
			{
			block = ( G00.Cond()
						+ X.AbsCond( xe )
						+ Y.AbsCond( ye )
						+ ToolRot);
			}
			else
			{
			block = ( G00.Cond()
						+ X.AbsCond( clfile.Xe() )
						+ Y.AbsCond( clfile.Ye())
						+ ToolRot);

			}
			CondOutput( block );
//			OutputDump( block );



		}

		double dx = xe - X.Curr();
		double dy = ye - Y.Curr();





		if(DropMAN) Drop();
		ConditionalSlugsDump();
		isTop = (dbEntity.IntGet("Top") == 1);
		if(isTop)
		{
			CondOutput("TC_TOOL_POSITION(TOP)" );
			isTop = false;
		}
		m_time += (Math.sqrt( dx*dx + dy*dy ) / RAPID_RATE);

		m_time += PUNCH_HIT_TIME;

	}

	private double EffectiveArea()
	{
		DbTool	dbTool;
		double	radius;

		dbTool = clfile.Entity().Tool();
		int type = clfile.Int( "Type_ID" );

		switch (type)
		{
		case  Const.CENTER_PUNCH:
		case  Const.ROUND:
			radius = (0.5 * dbTool.DoubleGet("Diameter")) * Const.PI;
			break;
		case  Const.SQUARE:
			radius = dbTool.DoubleGet("Width") * Const.PI; // was  dbTool.DoubleGet("Width");
			break;
		case  Const.RECTANGLE:
			radius = dbTool.DoubleGet("Length") * dbTool.DoubleGet("Width");
			break;
		case  Const.OBROUND:
			radius = dbTool.DoubleGet("Length") * dbTool.DoubleGet("Width");
			break;
		case  Const.DIAMOND:
			radius = dbTool.DoubleGet("Length") * dbTool.DoubleGet("Width");
			break;
		case  Const.CORNER_RADIUS:
			radius = dbTool.DoubleGet("Length") * dbTool.DoubleGet("Width");
			break;
		case  Const.SINGLE_D:
			radius = (0.5 * dbTool.DoubleGet("Diameter")) * Const.PI;
			break;
		case  Const.DOUBLE_D:
			radius = (0.5 * dbTool.DoubleGet("Diameter")) * Const.PI;
			break;
		case  Const.TRAPEZOID:
			radius = dbTool.DoubleGet("Length") * dbTool.DoubleGet("Width");
			break;
		case  Const.KEYHOLE:
			radius = dbTool.DoubleGet("Length") * dbTool.DoubleGet("Width");
			break;
		case  Const.FORMING:
			radius = 0;
			break;
		case  Const.MARKING:
			radius = 0;
			break;
		case  Const.HEXAGON:
		default:					radius =  0; break;
		}

		return radius;
	}


	private String Angle( double degrees )
	{
		String angle = "";

		if ( C.Delta( degrees ) )
		{
			angle = " C1=DC(" + C.Uncond( degrees ) + ")";
		}

		return angle;
	}

	private void ConditionalSlugsDump()
	{
		double	radius, area, volume;
		double  SOME_LIMIT = 8661;// think it should be 220000

		area = EffectiveArea();  // or perhaps EffectiveArea()
//		area =  Const.PI * radius;
		volume = (area * Model.DoubleGet("Thickness")) * m_hits;

		m_volume = m_volume + volume;

		if (m_volume > SOME_LIMIT)
		{
//			CondOutput(	PUNCH.Cond(0));
//			CondOutput(	NIBBLE.Cond(0));
			CondOutput(" PUNCH_OFF" );
			CondOutput(" TC_SUCTION_OFF" );
			CondOutput(" G04 F3" );
			CondOutput(" TC_SUCTION_ON" );
			CondOutput(" PUNCH_ON" );
			m_volume = 0;

		}


	}


	private void Engage()
	{
		if ( !m_engaged )
		{
			int toolNo = clfile.Int("NC_Code_Number");
			String block;
			DbEntity dbEntity = clfile.Entity( );
			double xs = clfile.Xs() * m_cf + newrepox;
			double ys = clfile.Ys() * m_cf + newrepoy;

			Point ps = new Point( X.Curr(), Y.Curr(), 0. );
			Point pe = new Point( xs, ys, 0. );
			int doff = clfile.Int("Doff");
			boolean avoided = ConditionalClampsAvoid( ps, pe );
			boolean DropMAN = false;
			DropMAN = (dbEntity.IntGet("DropMAN") == 1);
			if(DropMAN != true) DropMAN = (dbEntity.IntGet("DropMAN") == 2);
			if (DEBUG) output.Dump( "====>Engage()" );
			C.Set(500);
//				output.Dump( "====>multishear()" + doff);
//				CondOutput(doff);
			if ( IsMarkingTool() )
			{
				block = Rapid( xs, ys, true );
				block += "C1=DC(" + C.Cond(IndexAngle()) + ")";
				CondOutput( block );
				CondOutput( MARK.Cond(1) );
			}
			if ( IsPunchTool() )
			{
				block = Rapid( xs, ys, true );
				if(isHole)
				{

					CondOutput( NIBBLE.Cond(0) );
					if(DropMAN == true) CondOutput( PUNCH.Cond(0) );
					if(DropMAN != true) CondOutput( PUNCH.Cond(1) );
					block += "C1=DC(" + C.Cond(IndexAngle()) + ")";
					CondOutput( block );
					NewAngle = IndexAngle();
				}
				else
				{

					block += "C1=DC(" + C.Cond(IndexAngle()) + ")";
					CondOutput( PUNCH.Cond(0) );
					CondOutput( block );
					CondOutput( NIBBLE.Cond(1) );
					CondOutput( " SPP=" + Feedrate() );
				}
//				if ( AutoIndex() )
//					block += "C1=DC(" + C.Cond(IndexAngle()) + ")";


			}
			if(doff == 19)
			{
				CondOutput( BEAD.Cond(0) );
				block = Rapid( xs, ys, false );

				if ( AutoIndex() )
					block += "C1=DC(" + C.Uncond(IndexAngle()) + ")";
				CondOutput( block );
				if(m_engaged == false )
				CondOutput( BEAD.Cond(1) + "(\"PTT-" + passPTT + "\")" );
			}
			if(doff == 27)
//			if(toolNo == 7620500)
			{
//				CondOutput( MULTISHEAR.Cond(0) );
				block = Rapid( xs, ys, false );
//				output.Dump( "====>multishear()" );
				if ( AutoIndex() )
					block += "C1=DC(" + C.Uncond(IndexAngle()) + ")";
				CondOutput( block );
				if(m_engaged == false )
				CondOutput( MULTISHEAR.Cond(1) + "(\"PTT-" + msptt + "\")" );
				sett = 1;
				dbEntity = clfile.Entity( );
				DropMAN = (dbEntity.IntGet("DropMAN") == 1);
		//		if(DropMAN != true) DropMAN = (dbEntity.IntGet("DropMAN") == 2);
				if(DropMAN) dcount++;
			}
			m_engaged = true;
		}

	}

	private void Disengage()
	{
		DbEntity dbEntity = clfile.Entity( );
		if (DEBUG) output.Dump( "====>DisEngage()" );
		int doff = clfile.Int("Doff");
		if ( m_engaged )
		{
			X.Set(49);
			Y.Set(-.001);
			C.Set(400);
			CondOutput(NIBBLE.Cond(0));
			CondOutput(TAP.Cond(0));
			CondOutput(PUNCH.Cond(0));
	//		CondOutput(PUNCH.Cond(0));
			CondOutput(BEAD.Cond(0));
			CondOutput(MARK.Cond(0));

			if(doff == 27)
			{
				CondOutput( MULTISHEAR.Cond(0) );
//				CondOutput( " TC_SHEET_TECH(\"SHT-1\")" );


//				CondOutput( " we are here " + DropMAN );

			}
			if(dcount == 1)
			{
//				CondOutput( " we are here -------------------" + DropMAN );
				CondOutput(" TC_PART_UNLOAD(\"PAU-1\")");
				 dcount = 0;
			}

			m_engaged = false;

		}

	}



	private boolean IsPunchTool()
	{
		int type = clfile.Int( "Type_ID" );
		return IsPunchTool( type );
	}

	private boolean IsPunchTool( int type )
	{
		int	doff= clfile.Int("Doff");
		if (doff == 88)
		{
			doff = 0;
		}
		return ((type >= Const.ROUND && type <= Const.KEYHOLE) || type == Const.HEXAGON || (type == Const.CUSTOM && doff == 0));
//		return (type == Const.ROUND );
	}
	private boolean IsMarkingTool()
	{
		int type = clfile.Int( "Type_ID" );
		return IsMarkingTool( type );
	}

	private boolean IsMarkingTool( int type )
	{
		return (type == Const.MARKING);
	}
	private boolean IsTapTool()
	{
		int type = clfile.Int( "Type_ID" );
		return IsTapTool( type );
	}

	private boolean IsTapTool( int type )
	{
		return (type == Const.TAP);
	}

	private boolean IsFormingTool()
	{
		int type = clfile.Int( "Type_ID" );
		return IsFormingTool( type );
	}

	private boolean IsFormingTool( int type )
	{
		int	doff= clfile.Int("Doff");
		return (type == Const.FORMING || (type == Const.CUSTOM && doff > 0));
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

//	private double IndexAngle()
//	{
//		return clfile.Dbl( "orient" );
	private double IndexAngle()
	{
		final double TOL = 1.e-2;
		DbEntity dbEntity;
		DbCurve dbCurve;
		DbTool dbTool;
		boolean autoIndex;
		int type;
		double currAng  = 0.0;

		double nextAng ;
		double delta;

		currAng = C.Curr();	// default to something
		nextAng = currAng;

		dbEntity = clfile.Entity();
		dbTool = dbEntity.Tool();
		autoIndex = (dbTool.IntGet("Auto_Index") == 1);
		type = dbTool.IntGet("Type_ID");
		int doff = clfile.Int("Doff");
//		if ( autoIndex )
//		{
			nextAng = dbEntity.DoubleGet("orient");
			if (nextAng < -.01)
			{
				nextAng = nextAng + 360;
				if (nextAng < -.01)
				{
					nextAng = nextAng + 360;
				}
			}
			if (nextAng > 360.09)
			{
				nextAng = nextAng - 360;// added 9/20/04
			}

			if (nextAng < 360.09 & nextAng > 359.9 )  // ie. less-than Const.UNKNOWN
			{
				nextAng = 0.0;// simply use the orientation attached to the entity.
			}
			if (nextAng < 360 & nextAng >=0 )  // ie. less-than Const.UNKNOWN
			{
				// simply use the orientation attached to the entity.
			}
			else
			{
				// calculate the orientation at the start of the entity
				dbCurve = DbCurve.DbCurve( dbEntity );
				if (dbCurve == null)
				{
					nextAng = 0.0;
				}
				else
				{
					nextAng = dbCurve.StartTan().Radians() * Const.RAD2DEG;
//					output.Dump( "====> Index()" + "****" + nextAng);
				}
			}

//		output.Dump( "====> Index()" + "****" + nextAng);


//		}

		delta = Math.abs( nextAng - currAng );
		if (delta > TOL)
		{
			// Limit head rotations by considering tool symmetry.
			switch (type)
			{
				case Const.RECTANGLE:
					nextAng = nextAng % 180.;
					break;
	//			case Const.TRAPEZOID:
	//				nextAng = nextAng + 180.;
	//				break;
				case Const.ROUND:
					nextAng = nextAng % 90.;
					break;
				case Const.OBROUND:
				case Const.DIAMOND:
				case Const.DOUBLE_D:
				case Const.KEYHOLE:
					nextAng = nextAng % 180.;
					break;

				case Const.SQUARE:
					nextAng = nextAng % 180.;
					break;

				case Const.HEXAGON:
					nextAng = nextAng % 60.;
					break;

				default:
					break;
			}
		}
			if(doff == 27)
			{

					dbCurve = DbCurve.DbCurve( dbEntity );
					nextAng = dbCurve.StartTan().Radians() * Const.RAD2DEG;
//					output.Dump( "====> Index()" + "****" + nextAng);


				if(nextAng >= 20.0)
				{
					nextAng = nextAng - 20;
				}
				else
				{
					nextAng = nextAng + 340;
				}
				if(nextAng == 340|| nextAng == 70|| nextAng == 160||nextAng == 250)
				{
//					Msg.Display("Can to have tool at this angle:-" + nextAng);
				}
				else
				{
					output.Dump( "STOP-------------------------" );
					Msg.Display("Can to have tool at this angle:-" + nextAng);
				}
			}
/*
% is the modulo operator
eg . 25 % 10 = 5    23 % 3 = 2
*/
		return nextAng;
	}


	private void Stripper()
	{
		if (m_stripper == 0)
			CondOutput( "G177M55(NORMAL STRIPPER HEIGHT)" );
		else if (m_stripper == 1)
			CondOutput( "M56(EXTENDED STRIPPER HEIGHT)" );
		else
			CondOutput( "M56(TOOL CHANGE STRIPPER HEIGHT)" );
	}

	private String CNCFile()
	{
		String CNCFileName = "VIEWER";
		String CNCFile = output.Path();
		if (CNCFile != null)
		{
			int PeriodPos = CNCFile.indexOf('.');
			int SlashPos = CNCFile.lastIndexOf('\\') + 1;
			if (SlashPos >= 0 && PeriodPos > SlashPos)
				CNCFileName = CNCFile.substring(SlashPos,PeriodPos);
		}
		return CNCFileName;
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
		rpo++;
//		int repo = clfile.Int( "repo" );
		int repo = clfile.Int( "_zone_repo" );
		repoLocation = clfile.Xe();

//		newrepox = mynewrepox - repoLocation;

		if (DEBUG) output.Dump( "====>Repo-hold()" + rpo);
		repoY = 0.050;

		if ( Trpo)
		{
			if (rpo == 1)
			{
				output.Dump( "DA,'RPO-" + rpo + "'," + repoLocation + ",0.05,1181.10,0,1,1,0.00" );
//					newrepoy = 0.05;
				loca = loca + repoLocation;
			}
			else
			{
				if(repoLocation == 0.0)
				{
					loca = 0 - loca;
					if(loca == 0.0)
					{
						loca = repo;
					}
//					loca = clfile.Xe();
//	output.Dump( "------loca-------------" + loca);
					output.Dump( "DA,'RPO-" + rpo + "'," + loca + ",0.0,1181.10,0,1,1,0.00" );
					newrepox = mynewrepox + repoLocation;


				}
				else
				{

					repoLocation = clfile.Xe() - mynewrepox;
//	output.Dump( "-------------------" + repoLocation);
					output.Dump( "DA,'RPO-" + rpo + "'," + repoLocation + ",0.0,1181.10,0,1,1,0.00" );
					loca = loca + repoLocation;
					newrepox = mynewrepox + repoLocation;
				}
			}

		}
		mynewrepox = repoLocation;
	}

	private void Repo()
	{
		// NOTE: In this case, clfile.Xe() is an incremental distance
		// as extracted from the @REPO attribute 'x' parameter.
		DbEntity dbEntity = clfile.Entity( );
		rpo++;
		m_time += REPO_TIME;
		int repo = clfile.Int( "_zone_repo" );
		repoLocation = clfile.Xe();
		double delta = repoLocation - G53X.Curr();
		double mynewrepox = 0;

		if (DEBUG) output.Dump( "====>Repo------()" + rpo);
		CondOutput( PUNCH.Cond(0) );
		CondOutput(NIBBLE.Cond(0));
		CondOutput( " G53" + X.AbsUncond( m_xHold ) + Y.AbsUncond( m_yHold ) ); // #XPOS Y#YPOS
		CondOutput( " TC_SHEET_REPOSIT(\"RPO-" + rpo + "\")" );

			if ( isPressureFoot == true)
			{
				CondOutput( " PRESSERFOOT_ON" );
			}
			else
			{
				CondOutput( " PRESSERFOOT_OFF" );
			}
//		CondOutput( " PRESSERFOOT_OFF" );
		CondOutput( PUNCH.Cond(1) );


		NewAngle = 500;
			if (rpo == 1)
			{
				mynewrepox = repoLocation;
				newrepoy = 0.05;
				repoLocation = clfile.Xe();
				newrepox = repoLocation;
			}
			else
			{
				repoLocation = clfile.Xe();

				newrepox = repoLocation;


			}
	oldrepolocation = newrepox;
	int toolNo = clfile.Int("NC_Code_Number");
	if(toolNo == 7620500)
	{
		Msg.Display( " WE can not REPO with the Multi Shear Tool");
		output.Dump( "STOP------------" );
	}

	seqtool = 0;
	}

	private void Drop()
	{
		DbEntity dbEntity = clfile.Entity( );
		boolean DropMAN1;
		boolean DropMAN2;
		DropMAN1 = (dbEntity.IntGet("DropMAN") == 1);
		DropMAN2 = (dbEntity.IntGet("DropMAN") == 2);


		if(DropMAN1) CondOutput(" TC_PART_UNLOAD(\"PAU-1\")");
		if(DropMAN2) CondOutput(" TC_PART_UNLOAD(\"PAU-2\")");
 		CondOutput( PUNCH.Cond(1) );
			X.Set(0);							;
			Y.Set(0);
		NewAngle = 500.0;
			C.Set(500);



	}


	private void RPOTable()
	{

		clfile.StatePush();
		while ( !clfile.AtEnd() )
		{
			clfile.Read();
				switch ( clfile.RecType() )
				{
				case Clfile.eRepoCommand:
				Hold();
				}


		}

		clfile.StatePop();
		output.Dump( "ZA,DA, " + rpo );

		rpo = 0;
		Trpo = true;
		clfile.StatePush();
		while ( !clfile.AtEnd() )
		{
			clfile.Read();
				switch ( clfile.RecType() )
				{
				case Clfile.eRepoCommand:
				Hold();
				}


		}

		clfile.StatePop();

		rpo = 0;

	}



	private void Stop()
	{
		if (DEBUG) output.Dump( "====> Stop()" );

		CondOutput( M18.Cond() );  //  4/6/01

		if ( clfile.Dbl( "dx" ) > 15.0 && m_slidemovelarge == 1)
		{
			CondOutput("(SLIDE MOVE TO FOLLOW)");
			output.Dump( SeqNum() + G01.Uncond() + X.AbsCond( X.Curr() + 7.00 ) + "F400." );
		}

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
		int repo = clfile.Int( "repo" );
		if (repo != 1)
			return;

		int indx = clfile.Int( "num" ) - 1;
		if (indx < 0 || indx > 1)
			return;

		if (m_clamp[indx] != null)
			return;

       	double xmin = clfile.Dbl( "xmin" );
		double ymin = clfile.Dbl( "ymin" );
		double xmax = clfile.Dbl( "xmax" );
		double ymax = clfile.Dbl( "ymax" );

		double x = clfile.Xe();
		double y = clfile.Ye();

		if (DEBUG)
		{
			output.Dump( "====> ClampInfo()" );
			output.Dump( "indx:" + indx
						+ " xmin:" + xmin + " ymin:" + ymin
						+ " xmax:" + xmax + " ymax:" + ymax );
		}

		m_clamp[indx] = new SafeZone();

		m_clamp[indx].Set( x, y, xmin, (ymin - ymax), xmax, 0. );
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
		double xmin = clamp.Xmin() + repoAmount;
		double ymin = clamp.Ymin();
		double xmax = clamp.Xmax() + repoAmount;
		double ymax = clamp.Ymax();

		// NOTE: SafeZone.m_x & SafeZone.m_y are not really used.
		clamp.Set( 0., 0., xmin, ymin, xmax, ymax );
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
		die = clfile.Str( "cg.die" );



		// cg.xload|double|Load X|0.0
		// cg.yload|double|Load Y|0.0
		m_xLoad = Model.DoubleGet( "cg.xload" );
		m_yLoad = Model.DoubleGet( "cg.yload" );



		m_engaged = false;
	}


	private double Feedrate()
	{
//		return clfile.Int( "feed" );
		int type = clfile.Int( "Type_ID" );
		double diameter;
		diameter = clfile.Dbl("Diameter");
		double width;
		width= clfile.Dbl("Width");
		double length;
		length= clfile.Dbl("Length");
		double myFeed = 1;

		if (type == Const.ROUND)			myFeed =  diameter * .1;
		if (type == Const.SQUARE)			myFeed =  width - .05;
		if (type == Const.RECTANGLE)		myFeed =  length - .05;
		if (type == Const.OBROUND)		myFeed =  length - width - .05;
		return myFeed;
	}

	private void SymbolsInit()
	{
		// For reference:
		//     AddrFmt( addr, nzSign, nzLeadZeros, nzAbscissa, nzDecimal,
		//                nzTrailZeros, nzMantissa, zFormat)

        if (m_metric)
		{
			X = new DimAddr(  " X", 5, 1, 4, 1, 1, 2, "0.0" );
			Y = new DimAddr(  " Y", 5, 1, 4, 1, 1, 2, "0.0" );
			G53X = new DimAddr(  "G53 X", 6, 1, 4, 1, 1, 3, "0" );
			I = new MiscAddr( " I", 5, 1, 4, 1, 0, 3, "0.0" );
			J = new MiscAddr( " J", 5, 1, 4, 1, 0, 3, "0.0" );
 			L = new MiscAddr(  " Y", 6, 1, 4, 1, 1, 3, "0" );
		}
		else
		{
			X = new DimAddr(  " X", 5, 1, 4, 1, 1, 3, "0.0" );
			Y = new DimAddr(  " Y", 5, 1, 4, 1, 1, 3, "0.0" );
			G53X = new DimAddr(  "G53 X", 6, 1, 3, 1, 1, 4, "0" );
			I = new MiscAddr( " I", 5, 1, 2, 1, 0, 3, "0.0" );
			J = new MiscAddr( " J", 5, 1, 2, 1, 0, 3, "0.0" );
			L = new MiscAddr(  " Y", 6, 1, 2, 1, 1, 2, "0" );
		}

		N = new MiscAddr( "N", 0, 0, 4, 0, 0, 0, "0" );
		C = new MiscAddr( "",  5, 1, 3, 1, 0, 2, "0.0" );
		T = new MiscAddr( "T", 0, 0, 3, 0, 0, 0, "0" );
		D = new MiscAddr( "D", 0, 0, 2, 0, 0, 0, "00" );
		F = new MiscAddr( "", 0, 0, 5, 1, 0, 0, "0" );
		F2 = new MiscAddr( "#512=", 0, 0, 5, 1, 0, 0, "0" );
		CT = new MiscAddr( " ", 5, 0, 5, 1, 0, 3, "0" );

		UNITS  = new Group( null );
		ABSINC = new Group( null );
		COMP   = new Group( null );

		PRESS  = new Group( null );
		STOP   = new Group( null );
		PUNCH  = new Group( null );
		LASER  = new Group( null );
		TAP    = new Group( null );
		NIBBLE  = new Group( null );
		MULTISHEAR  = new Group( null );
		LASER  = new Group( null );
		MARK  = new Group( null );
		BEAD  = new Group( null );
		TAP    = new Group( null );
		LOAD   = new Group( null );
		PIERCE = new Group( null );
		MOTION = new Group( null );
		LINEAR = new Group( MOTION );
		ARC    = new Group( MOTION );

		G00 = new Symbol( LINEAR, " G01", Clfile.eRapid );
		G01 = new Symbol( LINEAR, " G01", Clfile.eLine );
		G02 = new Symbol( ARC,    " G02", Clfile.eCwArc );
		G03 = new Symbol( ARC,    " G03", Clfile.eCcwArc );
		GRESET = new Symbol( MOTION, "", -1 );

		G20 = new Symbol( UNITS,  " G20",  0 );	// inch
		G21 = new Symbol( UNITS,  " G21",  1 );	// metric

		G40 = new Symbol( COMP,   " G40",  0 );	// off
		G41 = new Symbol( COMP,   " G41",  1 );	// left
		G42 = new Symbol( COMP,   " G42", -1 );	// right

		G90 = new Symbol( ABSINC, " G90",  0 );	// absolute
		G91 = new Symbol( ABSINC, " G91",  1 );	// incremental

		M00 = new Symbol( STOP,   " M00",  0 );	// hard stop
		M01 = new Symbol( STOP,   " M01",  1 );	// optional stop

		M40 = new Symbol( PRESS,  " M40",  0 );	// marking

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

		PUNCH_OFF = new Symbol( PUNCH, " PUNCH_OFF", 0 );
		PUNCH_ON  = new Symbol( PUNCH, " PUNCH_ON",  1 );

		TC_TAP_OFF = new Symbol( TAP, " TC_TAP_OFF", 0 );
		TC_TAP_ON  = new Symbol( TAP, " TC_TAP_ON",  1 );

		NIBBLE_OFF = new Symbol( NIBBLE, " NIBBLE_OFF", 0 );
		NIBBLE_ON  = new Symbol( NIBBLE, " NIBBLE_ON",  1 );

		TC_MULTISHEAR_OFF = new Symbol( MULTISHEAR, " TC_MULTISHEAR_OFF", 0 );
		TC_MULTISHEAR_ON  = new Symbol( MULTISHEAR, " TC_MULTISHEAR_ON",  1 );

		TC_BEAD_OFF = new Symbol( BEAD, " TC_BEAD_OFF", 0 );
		TC_BEAD_ON  = new Symbol( BEAD, " TC_BEAD_ON",  1 );

		TC_MARK_OFF = new Symbol( MARK, " TC_MARK_OFF", 0 );
		TC_MARK_ON  = new Symbol( MARK, " TC_MARK_ON",  1 );

	}

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Declarations
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	Clfile clfile = null;
	OutSys output = null;

	boolean m_metric	= false;
	boolean m_incr		= false;

	int m_currToolNo = 0;
	int m_drillToolNo = 0;

	DbTool m_currTool = null;

	double m_time = 0;
	double l_time = 0;

	private double m_xHold = 0.0;
	private double m_yHold = 0.0;


	String	m_torchCode		= null;
	String	die				= null;
	String	prog		= null;
	double	me3			= 20.0;
	double	me2			= 10.0;
	double	me4			= 30.0;
	double	me5			= 40.0;
	double	me6			= 50.0;
	boolean	ttool		=false;
	boolean	ftool		=false;
	boolean	ptool		=false;
	boolean	ltool		=false;
	boolean	doDrop		=false;
	boolean	Eqdrop		=false;
	boolean DropMAN = false;
	boolean	m_engaged		= false;
	boolean datable 	= false;
	boolean myDrop		= false;
	boolean Trpo		= false;
	boolean set			= true;
	boolean isPressureFoot	= false;
	boolean isTop	= false;
	boolean isHole		= false;

	double	m_cf			= 1.0;			// Units conversion factor
	double	m_xLoad			= 0.0;
	double	m_yLoad			= 0.0;
	double	m_Amps			= 0.0;
	double	NewAngle		= 360.0;
	double cutmin			= 0.0;
	double equalx			= 1.0;
	double equaly			= 1.0;
	double	m_volume	    = 0.0;
	double oldrepolocation		=0.0;
			double mynewrepox = 0;

	int		m_tableVelocity	= 0;
	int		m_pierce		= 0;
	int		m_slidemovesmall	= 0;
	int		m_slidemovelarge	= 0;
	int		m_stripper		= 0;
	int		m_matl			= 0;
	int		LastTool        = 0;
	int		globalCount		= 1;
	int		wedrop			= 2;
	int 	mydrop			= 0;
	int		Nowedrop		= 0;
	int		clamp1			= 0;
	int		clamp2			= 0;
	int		clamp3			= 0;
	int		rpo				= 0;
	int		dropcount		= 0;
	int 	passPTT			=0;
	int 	lftool			=0;
	int		multiS = 0;
	int		msptt = 0;
	int		sett = 0;
	int 	dcount =0;
	int		seqtool = 0;
	int		seqtlcount = 0;
	double newrepox     = 0;
	double newrepoy     = 0;
	double repoLocation     = 0;
	double repoY = 0.0;
	double loca = 0.0;

	Vector
	Count;

	static int	m_maxHits	= 100;
	double			m_hits		= 1;
	int 		NoTools		= 0;
	int		myRepo			= 0;

	DimAddr  X = null;
	DimAddr  Y = null;
	DimAddr  G53X = null;

	MiscAddr I = null;
	MiscAddr J = null;
	MiscAddr C = null;
	MiscAddr N = null;
	MiscAddr T = null;
	MiscAddr D = null;
	MiscAddr F = null;
	MiscAddr F2 = null;
	MiscAddr CT = null;
	MiscAddr L = null;

	Group UNITS  = null;
	Group TOR  = null;  //   4/6/01
	Group ABSINC = null;
	Group COMP   = null;
	Group PRESS  = null;
	Group OFFSET = null;
	Group STOP   = null;
	Group PUNCH  = null;
	Group LASER  = null;
	Group TAP    = null;
	Group NIBBLE  = null;
	Group MULTISHEAR  = null;
	Group BEAD  = null;
	Group MARK  = null;
	Group LOAD   = null;
	Group PIERCE = null;
	Group MOTION = null;
	Group LINEAR = null;
	Group ARC    = null;

	Symbol G00 = null;
	Symbol G01 = null;
	Symbol G02 = null;
	Symbol G03 = null;
	Symbol GRESET = null;

	Symbol G20 = null;
	Symbol G21 = null;

	Symbol G40 = null;
	Symbol G41 = null;
	Symbol G42 = null;

	Symbol G90 = null;
	Symbol G91 = null;

	Symbol M00 = null;
	Symbol M01 = null;

	Symbol M40 = null;
	Symbol M41 = null;
	Symbol M42 = null;

	Symbol M17 = null;  //  4/6/01
	Symbol M18 = null;  //  4/6/01
	Symbol M19 = null;  //  4/6/01


	Symbol M75 = null;
	Symbol M85 = null;
	Symbol G70 = null;

	Symbol M87 = null;
	Symbol M88 = null;
	Symbol M170 = null;

	Symbol M15 = null;
	Symbol M65 = null;

	Symbol PUNCH_OFF = null;
	Symbol PUNCH_ON  = null;

	Symbol TC_TAP_OFF = null;
	Symbol TC_TAP_ON  = null;

	Symbol TC_LASER_OFF = null;
	Symbol TC_LASER_ON  = null;

	Symbol NIBBLE_OFF = null;
	Symbol NIBBLE_ON  = null;

	Symbol TC_MULTISHEAR_OFF = null;
	Symbol TC_MULTISHEAR_ON  = null;

	Symbol TC_BEAD_OFF = null;
	Symbol TC_BEAD_ON  = null;

	Symbol TC_MARK_OFF = null;
	Symbol TC_MARK_ON  = null;

	DbEntity m_entity = null;

	SafeZone [] m_clamp = null;

	ToolData [] m_tool_data = null;
	int m_count = 0;

}



