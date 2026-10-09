// mac.fodder | nop | Notched Rectangle | 0.
// mac.ProfLen | dbl | Length | 30
// mac.ProfWid | dbl | Width | 25
// mac.fodder | nop | Notch| 0
// mac.ProfLen2 | dbl | Notch Length | .688
// mac.ProfWid2 | dbl | Notch Width | .688
// mac.fodder | nop | 45 Notch| 0
// mac.notch45 | dbl | 45 Notch Length | .468
// mac.Logo |dcombo| Logo|On|Off|On



import Weng.System.*;
import Weng.Modeler.*;
import Weng.Math.*;
import Weng.Geometry.*;
import Weng.CodeGen.*;
import java.io.*;
import java.util.*;

//This macro creates a Rectangle with or without Radius corners. A negative value for the
//corner radius will produce a concave corner
public class PegBoardMXNest_R10 implements WengMacro
{
		private final boolean DEBUG = false;
		int UNDEFINED = (int) Const.UNDEFINED;
		String DOUBLE_QUOTE = "\"";

		double 	ClampAdjustX;
		double 	ClampAdjustY;
		double 	TopBorder = .75;
		double 	RepoAmt = -9;
		double 	splitdist = .05;
		Double 	DeadZoneBuffer = .02;
		int 	CMD_size = 20;

		//VarSys.Flush();
		int clamp1 = 5;//Model.IntGet("mac.clmp1")
		int clamp2 = 10;//Model.IntGet("mac.clmp2");


		double xref = 0;
		double yref = 0;
		double splitval=0;
		double splitlen = 0;

		double xc;
		double yc;


		double repoLocation = 0;

		int nPanelQty = Model.IntGet("mac.qty");
		double proflen = Model.DoubleGet( "mac.ProfLen" );
		double profwid  = Model.DoubleGet( "mac.ProfWid" );
		double orient = Model.DoubleGet( "mac.Orient" );
		double notchlength = Model.DoubleGet( "mac.ProfLen2" );
		double notchwidth = Model.DoubleGet( "mac.ProfWid2" );
		double len45 = Model.DoubleGet("mac.notch45");
		int nDoNotch = Model.IntGet("mac.Notch");
		double dHoleDia = Model.DoubleGet( "mac.dia");
		double DeadZoneWid = Model.DoubleGet("Clamp_Width");
		double DeadZoneLen = Model.DoubleGet("Clamp_Length");

		double FirstSqY;
		double PointB_Loc;

		DbEntity Notch_1_45;
		DbEntity Notch_3_45;
		double twicelen45 = len45 * 2 ;
		double twicenotchwidth = notchwidth * 2 ;
		double dMoveFromX, dMoveFromY;

		Point ptLogo = new Point(0,0,0);
		DbLine LogoLine, RtLine,LeftName, TopName, RightName, BottomName, ProfileStartName;
		DbEntity LeftLN;
		DbEntity TopLN;
		DbEntity RightLN;
		DbEntity BottomLN;
		DbEntity First45LN;
		DbEntity ProfileStartLN;
		DbEntity PointBent;
		DbLine First45;
		Point PointB;
		DbHole HoleB;



		DbTool Cluster = Model.ToolByStation(945871710);//( 945871710 )(1020);
		String cluster_name = Cluster.Name();

		DbTool RND_265 = Model.ToolByStation(10265);//( 10265 )(1016);
		DbTool Rect2_90X125 = Model.ToolByStation(4737032);//( 10265 )(1016);
		DbTool LogoTool = Model.ToolByStation(813750380);//( 10265 )(1016);
		DbTool Sqr1000 = Model.ToolByStation(41000);//( 10265 )(1016);
		DbTool Sqr373 = Model.ToolByStation(40373);//( 10265 )(1016);
		DbTool GridTL = Model.ToolByStation(10265);//( 10265 )(1016);
		DbTool DummyTL = Model.ToolByStation(1111111111);//( 10265 )(1016);

		DbTool	magpanel;
		DbTool ShakerTab;

		DbPoint TestPoint;
		int TestPointID;

		DbTool dbToolF = Model.ToolByStation(40373);//( 10265 )(1016);

		DbLine HorzLine;
		Point LeftSmallBend;
		int foofirst45,foo2nd45, foo3rd45,foo4th45,foo5th45, foo6th45, foo7th45, foo8th45;
		int LeftID, TopID, RightID, BottomID, TempID;
		boolean bHoleWNotch;
		int nDoLogo;
		double matlength = Model.DoubleGet("Length");
		double matwidth = Model.DoubleGet("Width");

		Point TempStartPt;
		Point TempEndPt;
		Point TempMidPt;


		//Point LeftMidPt;
		double TempStartX, TempStartY, TempEndX, TempEndY, TempMidX, TempMidY;
		double TempGridXSpace;
		double TempGridYSpace;
		double LeftX, LeftY, RightX, RightY;
		String ncfile;
		String cgfile;
		DbLine Assoc1, Assoc2, Assoc3, Assoc4;

		double DoesClearDeadZone;


		//StartClamp Value From Greg used to be Calculated as.....
		//double pos1 = startclamp + ((130/25.4) * clamp1);
		//Because he was calulating from a metric value and converting it to inch
		//This being Said we can just calculat the "startclamp" as ...
		//-7.08711 + 5.1181 which calulates to -1.9690
		//to make things easier we are directly setting the value here
		//to avoid hardcoded values in the body of the macro.

		//this is how it used to be:  double startclamp = -7.08711; //This Represents the Load Position
		//We now change it to
		double startclamp = -1.9690; //This Represents the Load Position
		double pos1 = -7.08711 + (5.118 * clamp1);
		double pos2 = -7.08711 + (5.118 * clamp2) ;


		public void main()
	{

		try
		{

			Selector.Flush();

			String MM2File = Registry.StringGet( Registry.REGROOT + "\\Fabrication\\FileOpen", "LastMM2");
			String CNCFile = Registry.StringGet( Registry.REGROOT + "\\Customizations", "SpecNcFile");
			String JAVAFile =Registry.StringGet( Registry.REGROOT + "\\Customizations", "SpecJava");

			Model.DoubleSet("cg.xload", 14.955);
			Model.DoubleSet("cg.yload", 5.512);

			String path;
			path = MM2File;

			bCreatedBottom = false;

			CreatedZone2 = false;

			Point  origin = new Point(0,0,0);

			bHoleWNotch = Holes_W_Notch();



			CreateZone1();


			if (bHoleWNotch)
			{

				CreateOutsideProfile();

				DoClamps();


				if(DoLogo())
				{
					CreateLogoLine();

					CreateLogo();
				}

				Viewer.RefreshView();

				CreateBendlines();

				CreateLL_UL_NotchHits();

				Create45ToolPath();

				//CreatePanelSizeCommand();

				ClusterStuff();


				MovePartToCenter();

				DoesClearDeadZone = BottomMidPt.Y();

				Viewer.RefreshView();

				Viewer.FullView();

				IsBottomInDeadZone();

				DoesClearDeadZone = BottomMidPt.Y();


				Viewer.RefreshView();

				Viewer.FullView();

				Selector.Flush();

				Portal.Execute("view:refresh:regen=1");

				Selector.All( true );
				Selector.Restrictions( true );

				Selector.AddAll();
				Portal.Execute("Feature:Modify: id=" + Zone1ID + ",selected=1");

				Selector.Flush();

				SaveTheMM2(path);
				OpenTheMM2(path);

				//if(DoesClearDeadZone < (DeadZoneWid + DeadZoneBuffer))
				//{
					//AvoidClamps();
				//}

				Selector.Flush();

				//UpdateCommandSize();

				//CreateShakerTabs();



				DbEntity dbEntity;
				DbHole dbHole;
				int count, indx, type, color;
				int entityID = 0;

				if(CreatedZone2)
				{
					if (matlength < 98)
					{
						//OptimizeZone2Cluster();
					}
					else
					{
						//SequenceToolPath();

						Selector.Flush();

						Selector.All( false );

						Selector.Line( true);

						//Portal.Execute("selector:filter: encoded=16");

						Portal.Execute("*selector:select:id=" + Model.EntityGet("L_Trim1"));


						//Portal.Execute("*selector:select:id=" + Model.EntityGet("L_Trim2"));

////
////						Portal.Execute("*selector:select:id=" + Model.EntityGet("T_Trim1"));
////
						Portal.Execute("Seq:Move: id=" + Zone1ID + ",mode=0");
////
////						Portal.Execute("Seq:Insert: seqid=580,insba=1");
////						Portal.Execute("Seq:Move: id=580,mode=0");
////						Selector.Flush();

//

						//Portal.Execute("Seq:Insert: seqid=" + Zone1ID + ",insba=1");

					}

					//Selector.Flush();

				}
				else
				{

					//OptimizeZone1Cluster();

					Selector.Flush();

					//CheckSeq();

					Selector.Flush();


				}

			}
			else //PegBoard with no notche
			{
				CreateOutsidNoNotch();
					//Msg.Diagnostic( "Doing New hole: ");

				if(DoLogo())
				{
					CreateLogoLine();
					CreateLogo();
				}

			}

			CreateShakerTabs();


			Selector.Flush();

			Viewer.RefreshView();

			int FeatureCount;

			Portal.Execute("zone:get:_zone_num=2");

			Zone2ID = Portal.IntGet("id");

			 Portal.Execute("*Feature:Count:id=" + Zone2ID);
			 FeatureCount = Portal.IntGet("id");

			  //Msg.Display("Zone2ID is  :" + Zone2ID + " the zone 2 count is :" + FeatureCount);

			 if(FeatureCount <1)
			 {
			 	Portal.Execute("Zone:Update: id=" + Zone2ID + ",delete=1");
			 }

			Portal.Execute("create:empty:") ;// get rid of empty feature
			Msg.Display("The file name is < " + path + " >");

			SaveTheMM2(path);

			OpenTheMM2(path);

			//DoCodeGen();
			//VarSys.Flush();

		}
		catch (Exception e)
		{
			Msg.Display("Message:" + e);
			ExceptionPrinter.StackTracePrint( e );
		}
	}


	public long CheckSeq()
{
		DbEntity	dbSequence;
		int			count, indx, nSeqID, LastSeqID;
		LastSeqID= -1 ;

        //Msg.Display("INto CheckSeq");

        Portal.Execute("Seq:Init:");
        Portal.Execute("Seq:Iter_init:ID=0");

        nSeqID = Portal.IntGet("id");

        m_active_workzoneID = nSeqID;

        Portal.Execute("Seq:Iter_init:ID=" + m_active_workzoneID);

        while (nSeqID > 0)
        {
            LastSeqID = nSeqID;
        	Portal.Execute("Seq:Iter_Next:");
            nSeqID=Portal.IntGet("id");

        }
	//	Msg.Display("Out of loop");
        //Msg.Display("the id is  < " + LastSeqID + " > ");

        return LastSeqID;
}

	public long CheckSeqZone2()
{
		DbEntity	dbSequence;
		int			count, indx, nSeqID, LastSeqID;
		LastSeqID= -1 ;

        //Msg.Display("INto CheckSeq");

        Portal.Execute("Seq:Init:");
        Portal.Execute("Seq:Iter_init:ID=1");

        nSeqID = Portal.IntGet("id");

        m_active_workzoneID = nSeqID;

        Portal.Execute("Seq:Iter_init:ID=" + m_active_workzoneID);
        //Msg.Display("The active workzone in zone 2 is < " + m_active_workzoneID + " > ");

        while (nSeqID > 0)
        {
            LastSeqID = nSeqID;
        	Portal.Execute("Seq:Iter_Next:");
            nSeqID=Portal.IntGet("id");

        }
	//	Msg.Display("Out of loop");
        //Msg.Display("the id is  < " + LastSeqID + " > ");

        return LastSeqID;
}

private void optimize()
{

				int		count, indx;
			DbTool	dbTool;

			Selector.StateSave();

			Portal.Execute("Pattern:Explode:all=1");


			dbTool = Model.ToolByStation(945871710);

			if (DEBUG) Msg.Display( "Tool is :" + dbTool );

			Selector.Flush();
			Selector.All( false );
			Selector.Feature(true);
			Selector.Line(true);
			Selector.Arc(true);
			Selector.Profile(true);
			Selector.Tool( true );
			Selector.Restrictions( true );

			Selector.AddAllRefsTo( Cluster );

			count = Selector.Count();

			if (DEBUG) Msg.Display( "count:" + count );

			Portal.Execute("Seq:Insert: seqid=209,insba=0");

			Portal.Execute("CodeGen:Optimize:id=209,mode=0,adv=0,Optalgorithm=1," +
							"Slice_Opt=0,Sticky=100,BiDir=0,Seq_Opt=2," +
							"AllHolesFirst=1,HolesByToolOrder=1,ByCompletePart=2," +
							"HolesAcrossLocalNest=0");

			Selector.Flush();

//			Selector.All( false );
//			Selector.Hole( true );
//			Selector.Restrictions( true );
//			Selector.AddAll();
//
//			Portal.Execute("CodeGen:Optimize:id=1,mode=0,adv=0,Optalgorithm=1,Slice_Opt=4,Sticky=100,BiDir=1,Seq_Opt=0");
//
//			count = Selector.Count();
//
//			if (DEBUG) Msg.Display( "count:" + count );

			Selector.StateRestore();



}

private void SequenceToolPath()
{
	int nSeqID;
	DbEntity LineSeq1 = null;
	DbEntity LineSeq2 = null;
	DbEntity LineSeq3 = null;
	DbEntity LineSeq4 = null;
	DbEntity LineSeq5 = null;
	DbEntity LineSeq6 = null;
	DbEntity LineSeq7 = null;;


			Portal.Execute("zone:get:_zone_num=2");

			Zone2ID = Portal.IntGet("id");

			//Msg.Display("Zone2ID is  :" + Zone2ID );


			HoleToCheck = Model.EntityGet("Sqr1000_Hole1");

			DoesIterfer = Zone2LeftInterferance(HoleToCheck);

		//	Msg.Display("The interfer is <" + DoesIterfer + " >");

			if(DoesIterfer==false)
			{
				Selector.All( true );
				Selector.Restrictions( true );
				Selector.AddAll();

				Portal.Execute("Feature:Modify: id=" + Zone2ID + ",selected=1");

			}

			if(matlength > 98)
			{


				//LineSeq1 = LeftTrim.Get(0);
				//LineSeq2 = LeftTrim.Get(1);

				//LineSeq3 =  TopTrim.Get(0);
				//LineSeq4 =  TopTrim.Get(1);

				HideLayers();
				Selector.All( true );
				Selector.Restrictions( true );
				Selector.AddAll();

				Portal.Execute("Feature:Modify: id=" + Zone2ID + ",selected=1");

				Selector.Flush();


				Portal.Execute("selector:filter: encoded=112");
				Portal.Execute("view:refresh:regen=0");
				Portal.Execute("selector:selectbox: xmin=0,ymin=0,zmin=" + RepoAmt + ",xmax=29.6995,ymax=" + matlength + ",zmax=98");


				Portal.Execute("Feature:Modify: id=" + Zone1ID + ",selected=1");


				//	SelctBox(0,0,RepoAmt,matwidth);

				ShowLayers();

				Viewer.RefreshView();

				Viewer.FullView();



				OptimizeZone1Cluster();

				OptimizeZone2Cluster();

				//CodeGen:Optimize:id=339,adv=0,Optalgorithm=1,Slice_Opt=2,Sticky=4,BiDir=1,Seq_Opt=0
//
//				if(bCreatedBottom)
//				{
//
//					Selector.Add( BottomTrim.Get(0));
//					Selector.Add( BottomTrim.Get(1));
//
//				}

			}

				Selector.Flush();

				Portal.Execute("selector:select:id=" + Zone2ID);

				Selector.All( false );

				Selector.Line( true);
				Selector.Hole( true);


				//("Sqr1000_Hole1");
				//First45ToolPath

				Selector.Add( Model.EntityGet("First45ToolPath"));
				Selector.Add( Model.EntityGet("Second45ToolPath"));

				//Selector.Add(Model.EntityGet("Left_Trim");
				//Selector.Add(LeftTrim.Get(0));



//Msg.Display("Hello The LTrim ID is < " + L_Trim1.Id() + " >");

				//Selector.Add( L_Trim1);
				//Selector.Add( L_Trim2);

				//Selector.Add( RightTrim.Get(0));
				//Selector.Add( RightTrim.Get(1));

				//Msg.Display("The selected count is < " + Selector.Count() + " >") ;
//
//
//				Portal.Execute("Seq:Insert: seqid=" + Zone1ID + ",insba=1");
//				Portal.Execute("Seq:Move: id=" + Zone1ID + ",mode=0");
Selector.Flush();


				Selector.Add(Model.EntityGet("L_Trim1"));
//				Selector.Add(Model.EntityGet("L_Trim1"));
//
			//	Msg.Display("The selected count is < " + Selector.Count() + " >") ;
				Portal.Execute("Seq:Insert: seqid=524,refid=194,refba=0");

				Portal.Execute("Seq:Move: id=" + Zone1ID + ",mode=0");


}


private void SaveTheMM2(String MM2File)
{

	Model.ExportMM2(MM2File);

}

private void OpenTheMM2(String MM2)
{

	Model.ImportMM2(MM2);

}

private void IsBottomInDeadZone()
{
			BottomLN=Model.EntityGet("BottomLine");

			BottomName = DbLine.DbLine( BottomLN);
			BottomMidPt = BottomName.MidPt();

			TopLN=Model.EntityGet("TopLine");

			TopName = DbLine.DbLine( TopLN);
			TopMidPt = TopName.MidPt();

			if(BottomMidPt.Y() < DeadZoneWid + DeadZoneBuffer)
			{
				Portal.Execute("transform:move: sx=0,sy=" + TopMidPt.Y() + ",sz=0,ex=0,ey=" + (matwidth - TopBorder) + ",ez=0,copies=0");

			}

			Viewer.RefreshView();
			Viewer.FullView();

			//check the bottom again to see if it is in the deadzone
			BottomLN=Model.EntityGet("BottomLine");

			BottomName = DbLine.DbLine( BottomLN);
			BottomMidPt = BottomName.MidPt();

			if(BottomMidPt.Y() < DeadZoneWid + DeadZoneBuffer)

			{
				//Msg.Display(" FIrst Move I think");
				Portal.Execute("transform:move: sx=0,sy=" + BottomMidPt.Y() +",sz=0,ex=0,ey=0,ez=0,copies=0");
			}

			Viewer.RefreshView();
			Viewer.FullView();

			Viewer.RefreshView();

			Viewer.FullView();

}


private void CreatePanelSizeCommand()
{
			DbCommand	dbCommand;
			Model.ActiveToolSet( Sqr1000 );
			Portal.Execute( "create:command: x=" + TopMidPt.X() +",y=" + matwidth +",z=0,pos=2,angle=0,cmd=\"" + (proflen) + " x " + (profwid) + "\"");
			Cmd_ID = Portal.IntGet("id");

			dbCommand=Model.CommandGet(0);

			dbCommand.Name("PanelSize");
			dbCommand.Tool( DummyTL );

}
private void OptimizeZone1Cluster()
{

		int			count, indx, count2, indx2, type;
		int 		lLayerID;
		String		cmd;
		DbCommand	dbCommand;
		int			id;
		int 		The_ID;
		DbFeature	dbFeature = null;

		DbEntity dbEntity = null;

		count = Model.EntityCount(Const.TOOL);

		for (indx = 0; indx < count; ++indx)
		{
			DbTool dblayer = Model.ToolGet(indx);
			Portal.Execute("*selector:filter: encoded=128");

			//Msg.Display("WE found layer < " + dblayer.Name() + " > and the cluster name is <" + cluster_name + " >");

			if (dblayer.Name().equalsIgnoreCase(cluster_name))
			{

				Selector.AddAllRefsTo( Cluster );
				count2 = Selector.Count();


				indx = 0;

				while(count2 > indx)
				{
					dbEntity = Selector.Get( indx );
					The_ID = dbEntity.Owner().Id();

					type = dbEntity.Type();

					if(type == Const.HOLE)

					{

						if(The_ID != Zone1ID)
						{
							Selector.Remove( dbEntity );

							//Msg.Display("Why are we here");
							count2 = Selector.Count();
							indx = -1;

						}

					}
					++indx;
				}


				continue;
			}

		}

		String sTemp;
		int nSeqID;
		int nZoneNumber =-1;
		int nSeqZone1ID =  -1;
		int nSeqZone2ID =  -1;

	    Portal.Execute("*Seq:Init:");
        Portal.Execute("*Seq:Iter_init:ID=0");

        nSeqID = Portal.IntGet("id");

        while (nSeqID > 0)
        {


           nZoneNumber = nZoneNumber + 1;

            sTemp = "WorkZone " + (nZoneNumber);

            if(nZoneNumber == 0)
            {

            	nSeqZone1ID = nSeqID;

            	Selector.AddAllRefsTo( Cluster );
            	Portal.Execute("Admin:GlobalModel:");
            	cmd = "CodeGen:Optimize:id=" + nSeqZone1ID + ",mode=0,adv=0,Optalgorithm=1,Slice_Opt=2,Sticky=4,BiDir=1,Seq_Opt=0";
            	Portal.Execute(cmd);
            	Portal.Execute("Admin:GlobalModel:");

            }



            Portal.Execute("*Seq:Iter_Next:");
            nSeqID = Portal.IntGet("id");

        }


}

private void OptimizeZone2aCluster()
{
		int			count, indx, count2, indx2, type;
		int 		lLayerID;
		String		cmd;
		DbCommand	dbCommand;
		int			id;
		int 		The_ID;

		DbEntity dbEntity = null;


		//Msg.Display("Hello Optimize Zone2");

		Selector.Flush();

		Selector.All( false );
		Selector.Hole( true );
		Selector.Restrictions( false );
		Selector.AddAllRefsTo( Cluster );

		count = Selector.Count();
		//Msg.Display( "Cluster Optimize::HolesGet() -- count: " + count );

		Viewer.RefreshView();
		Viewer.FullView();

		indx = 0;


}


private void OptimizeZone2Cluster()
{
		int			count, indx, count2, indx2, type;
		int 		lLayerID;
		String		cmd;
		DbCommand	dbCommand;
		int			id;
		int 		The_ID;
		DbFeature	dbFeature = null;

		DbEntity dbEntity = null;

		count = Model.EntityCount(Const.TOOL);

		for (indx = 0; indx < count; ++indx)
		{
			DbTool dblayer = Model.ToolGet(indx);
			Portal.Execute("*selector:filter: encoded=128");

			if (dblayer.Name().equalsIgnoreCase(cluster_name))
			{

				Selector.AddAllRefsTo( Cluster );
				count2 = Selector.Count();


				indx = 0;

				while(count2 > indx)
				{
					dbEntity = Selector.Get( indx );
					The_ID = dbEntity.Owner().Id();

					type = dbEntity.Type();

					if(type == Const.HOLE)

					{

						if(The_ID != Zone2ID)
						{
							Selector.Remove( dbEntity );

							//Msg.Display("Why are we here");
							count2 = Selector.Count();
							indx = -1;

						}

					}
					++indx;
				}


				continue;
			}

		}

		String sTemp;
		int nSeqID;
		int nZoneNumber =-1;
		//int nSeqZone1ID =  -1;
		int nSeqZone2ID =  -1;

	    Portal.Execute("*Seq:Init:");
        Portal.Execute("*Seq:Iter_init:ID=0");

        nSeqID = Portal.IntGet("id");
        while (nSeqID > 0)
        {


           nZoneNumber = nZoneNumber + 1;

            sTemp = "WorkZone " + (nZoneNumber);

            if(nZoneNumber == 1)
            {

            	nSeqZone2ID = nSeqID;

            	Selector.AddAllRefsTo( Cluster );
            	Portal.Execute("Admin:GlobalModel:");
            	cmd = "CodeGen:Optimize:id=" + nSeqZone2ID + ",mode=0,adv=0,Optalgorithm=1,Slice_Opt=2,Sticky=4,BiDir=1,Seq_Opt=0";
            	Portal.Execute(cmd);
            	Portal.Execute("Admin:GlobalModel:");

            }

            Portal.Execute("*Seq:Iter_Next:");
            nSeqID = Portal.IntGet("id");

        }

				Selector.Flush();

				CheckSeq();

				Selector.Flush();


				Selector.All( false );

				Selector.Line( true);

//				Selector.Add( LeftTrim.Get(0));
//				Selector.Add( LeftTrim.Get(1));
//
//				Selector.Add( TopTrim.Get(0));
//				Selector.Add( TopTrim.Get(1));
//
//				Selector.Add( RightTrim.Get(0));
//				Selector.Add( RightTrim.Get(1));
//
//				if(bCreatedBottom)
//				{
//
//					Selector.Add( BottomTrim.Get(0));
//					Selector.Add( BottomTrim.Get(1));
//
//				}

				Viewer.RefreshView();
				//nSeqZone2ID
				Portal.Execute("Seq:Insert: seqid=" + nSeqZone2ID + ",insba=1" );
				Portal.Execute("Seq:Move: id=" + nSeqZone2ID + ",mode=0");

//				Portal.Execute("Seq:Insert: seqid=" + m_active_workzoneID + ",insba=1" );
//				Portal.Execute("Seq:Move: id=" + m_active_workzoneID + ",mode=0");


}

private void AvoidClamps()
{
	Viewer.RefreshView();

	Viewer.FullView();


			LeftLN=Model.EntityGet("LeftLine");

			LeftName = DbLine.DbLine( LeftLN);

			LeftStartPt = LeftName.StartPt();

			LeftMidPt = LeftName.MidPt();


	Selector.All( true );
	Selector.Restrictions( true );
	Selector.AddAll();



	//Msg.Display("Beginning avoid clamps");

	HoleToCheck = Model.EntityGet("Sqr1000_Hole20");



	DoesIterfer = RghtInterferance(HoleToCheck);

	if (DEBUG) Msg.Display("After First clamp check and doesinterfer is < " + DoesIterfer + " >");

	if(DoesIterfer==true)
	{
		//Msg.Display("la la la Right Interferance is < " + DoesIterfer + " > ");
		double X_MoveAmt;
		double X_MoveChk;
		Point HoleCenPt, FirstClampHoleCenPt;
		DbHole dbHole, FirstClampHole;
		dbHole = DbHole.DbHole(HoleToCheck);

		HoleCenPt = dbHole.CenterPt();
		Selector.All( true );
		Selector.AddAll();

		//X_MoveAmt = GetSecondClampLeft() - (HoleCenPt.X() + .702 + DeadZoneBuffer);
		X_MoveAmt = GetSecondClampLeft() - (HoleCenPt.X() + DeadZoneBuffer);


		if (DEBUG) Msg.Display( " X_MoveAmt is <  " + X_MoveAmt + " > and LeftMidPt.X() is < " + LeftMidPt.X() + " >" );

		X_MoveChk=(X_MoveAmt * -1);


//Msg.Display( " X_MoveChk is <  " + X_MoveChk + " > and LeftMidPt.X() is < " + LeftMidPt.X() + " >" );
		if((X_MoveChk) < LeftMidPt.X())
		{
			//Msg.Display("WE move on the next line");
			Portal.Execute("transform:move: sx=0,sy=0,sz=0,ex=" + (X_MoveAmt ) + ",ey=0,ez=0,copies=0");
		}
		else
		{
			CreateZone2();


			Selector.All( true );
			Selector.AddAll();

			Portal.Execute("Feature:Modify: id=" + Zone2ID + ",selected=1");

			Selector.Flush();
			Selector.Add(Model.EntityGet("Sqr1000_Hole1"));
			Selector.Add(Model.EntityGet("Sqr1000_Hole2"));
			Selector.Add(Model.EntityGet("Sqr1000_Hole3"));
			Selector.Add(Model.EntityGet("Sqr1000_Hole4"));
			Selector.Add(Model.EntityGet("Sqr1000_Hole5"));
			Selector.Add(Model.EntityGet("Sqr1000_Hole6"));
			Selector.Add(Model.EntityGet("Sqr1000_Hole7"));
			Selector.Add(Model.EntityGet("Sqr1000_Hole8"));
			Selector.Add(Model.EntityGet("Sqr1000_Hole9"));
			Selector.Add(Model.EntityGet("Sqr1000_Hole10"));
			Selector.Add(Model.EntityGet("Sqr1000_Hole11"));
			Selector.Add(Model.EntityGet("Sqr1000_Hole12"));
			Selector.Add(Model.EntityGet("First45ToolPath"));
			Selector.Add(Model.EntityGet("Secodnd45ToolPath"));
			Portal.Execute("Feature:Modify: id=" + Zone1ID + ",selected=1");

		//	Msg.Display("The second workzone id is < " + Zone2ID + " >");

			Selector.Flush();
		}
	}
	else
	{
		if(matlength > 98)
		{
			CreateZone2();
		}
	}

	//Msg.Display("The Left iterferance is < " + LeftInterferance(HoleToCheck) + " > ");

	HoleToCheck = Model.EntityGet("Sqr1000_Hole1");

	DoesIterfer = LeftInterferance(HoleToCheck);

	if(DoesIterfer==true)
	{

		Selector.All( true );
		Selector.AddAll();

		double X_MoveAmt;
		Point HoleCenPt, FirstClampHoleCenPt;
		DbHole dbHole, FirstClampHole;
		HoleToCheck = Model.EntityGet("Sqr1000_Hole1");
		FirstClampHole = DbHole.DbHole(HoleToCheck);
		FirstClampHoleCenPt = FirstClampHole.CenterPt();

		X_MoveAmt = GetFirstClampLeft() - (FirstClampHoleCenPt.X() + .5 + .530 );

		//Msg.Display("How about This Move");
		Portal.Execute("transform:move: sx=0,sy=0,sz=0,ex=" + X_MoveAmt + ",ey=0,ez=0,copies=0");

	}

}
public boolean RghtInterferance(DbEntity HoleCheck)
{
	boolean bHoleCheck;
	DbHole dbHole;
	Point HoleCenPt;
	bHoleCheck = false;

	dbHole = DbHole.DbHole(HoleCheck);

	HoleCenPt = dbHole.CenterPt();

	if(HoleCenPt.Y() < 2.843)
	{
		if((HoleCenPt.X()) > GetSecondClampLeft() && (HoleCenPt.X() ) < GetSecondClampRight())
		{
			bHoleCheck = true;
			//Msg.Display("we are in first check");
		}
		else if ((HoleCenPt.X() + .702) > matlength)
		{


			bHoleCheck = false;
			//Msg.Display("we are in Second check and bHoleCheck is < " + bHoleCheck + " >");


		}

	}
	else
	{
		//Msg.Display("we are in third check");;
		bHoleCheck = false;
	}


	return bHoleCheck;

}

public boolean LeftInterferance(DbEntity HoleCheck)
{

	boolean bHoleCheck;
	DbHole dbHole;
	Point HoleCenPt;

	dbHole = DbHole.DbHole(HoleCheck);

	HoleCenPt = dbHole.CenterPt();

	if((HoleCenPt.X() + .7072) > GetFirstClampLeft())
	{
		bHoleCheck = true;
	}
	else
	{
		bHoleCheck = false;
	}

	return bHoleCheck;

}

public boolean Zone2LeftInterferance(DbEntity HoleCheck)
{

	boolean bHoleCheck;
	DbHole dbHole;
	Point HoleCenPt;

	dbHole = DbHole.DbHole(HoleCheck);

	HoleCenPt = dbHole.CenterPt();

	if((HoleCenPt.X() + .7072) > (GetFirstClampLeft() -9 ))
	{
		bHoleCheck = true;
	}
	else
	{
		bHoleCheck = false;
	}

	return bHoleCheck;

}

	private void SequenceNumbersAssign( DbEntity dbSequence )
	{
		DbEntity	dbEntity;
		String		msg;
		int			seqnum;

		msg = DbSeqIterator.Init( dbSequence.Id() );

		if (msg == null)
		{
			seqnum = 0;

			while (true)
			{
				dbEntity = DbSeqIterator.Get();
				if (dbEntity == null)
					break;

				Msg.Diagnostic("The id is " + dbEntity.Id());

//				dbEntity.IntSet( "_expseq", seqnum );
//				++seqnum;

				DbSeqIterator.Next();
			}
		}
		else
		{
			Msg.Display( msg );
		}
	}
	private double GetFirstClampLeft()
	{
		double Clamp1Left;

		Clamp1Left = pos1 - (DeadZoneLen/2);

		return Clamp1Left;
	}

	private double GetSecondClampRight()
	{
		double Clamp2Right;

		Clamp2Right = pos2 + (DeadZoneLen/2);

		return Clamp2Right;


	}

	private double GetSecondClampLeft()
	{
		double Clamp2Left;

		Clamp2Left = pos2 - (DeadZoneLen/2);

		return Clamp2Left;


	}

	private double GetDeadZoneSafety()
	{
		return DeadZoneWid + DeadZoneBuffer;
	}

	private Point GetProfileStart()
	{
		ProfileStartLN=Model.EntityGet("First45");

		ProfileStartName = DbLine.DbLine( ProfileStartLN);

		return ProfileStartName.StartPt();

	}

	private Point GetProfileEnd()
	{
		BottomLN=Model.EntityGet("BottomLine");

		BottomName = DbLine.DbLine( BottomLN);

		TempEndPt = BottomName.StartPt();

		return TempEndPt;

	}


	private void CreateZone1()
	{
		String		cmd;
		int 		id;
//
//		Portal.Execute("zone:update:_zone_num=1,_zone_bottom=" + matwidth
//						+ ",_zone_left=-9,_zone_right="
//							+ matlength + ",_zone_repo=" + (RepoAmt * -1) + ",_zone_top=0,hold_type=0,_hold_x="
//							+ (matlength/2)+ ",_hold_y=" + matwidth/2);
//
//		Portal.Execute("zone:get:_zone_num=1");
//
//		Zone1ID = Portal.IntGet("id");

			Portal.Execute("zone:get:_zone_num=1");

			Zone1ID = Portal.IntGet("id");

			Portal.Execute("zone:update:id=" + Zone1ID + ",_zone_num=1,_zone_bottom=" + matwidth
							+ ",_zone_left=-9,_zone_right="
								+ matlength + ",,_zone_repo=" + (RepoAmt * -1) + ",_zone_top=0,hold_type=0,_hold_x="
								+ (matlength/2)+ ",_hold_y=" + matwidth/2);


		Selector.All( true );
		Selector.Restrictions( true );
		Selector.AddAll();

		Portal.Execute("Feature:Modify: id=" + Zone1ID + ",selected=1");

		Selector.Flush();


	}
	private boolean CreateZone2()
	{
		String		cmd;
		zoneBottom = 0.0;
		zoneLeft = 0.0;
		zoneRight = 0.0;
		zoneTop = 0.0;
		zoneNum = 2;


		Portal.Execute("zone:update:_zone_num=2,_zone_bottom=" + matwidth
						+ ",_zone_left=" + (RepoAmt) +",_zone_right="
							+ matlength + ",_zone_top=0,hold_type=0,_hold_x="
							+ (matlength/2)+ ",_hold_y=" + matwidth/2);
		Zone2ID = Portal.IntGet("id");

		//Msg.Display("CreateZone2 Zone2ID is  :" + Zone2ID);
		CreatedZone2 = true;
		return true;
	}

	private void CreateLeftShakerTab()
	{
		DbLine dbLine;
		DbFeature	dbFeature = null;
		int GrowId;
		int  id;
		int gary;
		double ActualPT;
		Point ActualStartPT;
		Point ActualEndPT;


		Viewer.RefreshView();
		Selector.Flush();


		ShakerTab = new DbTool("Shaker_Tab");
		ShakerTab = Model.ToolGet("Shaker_Tab");
		ShakerTab.Color( DbEntity.RGB(141, 179, 226) );

		LeftLN=Model.EntityGet("LeftLine");

		LeftName = DbLine.DbLine( LeftLN);

		TempStartPt = LeftName.StartPt();
		TempMidPt = LeftName.MidPt();
		LeftMidPt = TempMidPt;

		TempEndPt = LeftName.EndPt();

		TempStartX = TempStartPt.X();
		TempStartY = TempMidPt.Y() - (splitdist/2);

		TempPT = new Point(TempStartX,TempStartY, 0);

		ActualStartPT = new Point(TempStartX,TempStartPt.Y() + splitdist,0.0);


		dbLine = new DbLine(ActualStartPT, TempPT);

		TempID =  Portal.IntGet("id");
		dbLine.Color( DbEntity.RGB(141, 179, 226) );

		if(TempMidPt.X() > .125)
		{
			Portal.Execute("toolpath:autoindexoffset: toolid=" + Rect2_90X125.Id() + ",profid=" + TempID + ",dir =1,longside = 1");

			id = Portal.IntGet("id");

			L_Trim1 = new DbFeature();
			L_Trim1.Id( id );
			L_Trim1.Name("Left_Trim1");


		}
		else
		{
			Portal.Execute("toolpath:autoindexoffset: toolid=" + Sqr1000.Id() + ",profid=" + TempID + ",dir =1,longside = 1");

			id = Portal.IntGet("id");

			L_Trim1 = new DbFeature();
			L_Trim1.Id( id );
			L_Trim1.Name("Left_Trim1");

		}

		Selector.Flush();

		Selector.All(true);
		Selector.Add(Model.EntityGet("Left_Trim1"));
		Portal.Execute("Feature:Modify: id=" + Zone1ID + ",selected=1");

		TempStartY = TempMidPt.Y() + (splitdist/2);
		TempPT = new Point(TempStartX,TempStartY, 0);

		ActualEndPT = new Point(TempStartX,TempEndPt.Y() - splitdist,0.0);

		dbLine = new DbLine(TempPT, ActualEndPT);
		TempID =  Portal.IntGet("id");
		dbLine.Color( DbEntity.RGB(141, 179, 226) );

		if(TempMidPt.X() > .125)
		{
			Portal.Execute("toolpath:autoindexoffset: toolid=" + Rect2_90X125.Id() + ",profid=" + TempID + ",dir =1,longside = 1");

			id = Portal.IntGet("id");
			Portal.Execute("*entity:type: id=" + id);

			LeftTrim2 = new DbFeature();
			LeftTrim2.Id( id );
			LeftTrim2.Name("Left_Trim2");

		}
		else
		{
			Portal.Execute("toolpath:autoindexoffset: toolid=" + Sqr1000.Id() + ",profid=" + TempID + ",dir =1,longside = 1");

			id = Portal.IntGet("id");
			LeftTrim2 = new DbFeature();
			LeftTrim2.Id( id );
			LeftTrim2.Name("Left_Trim2");
		}

		Selector.Flush();

		Selector.All(true);
		Selector.Add(Model.EntityGet("Left_Trim2"));
		Portal.Execute("Feature:Modify: id=" + Zone1ID + ",selected=1");

		Selector.Flush();
	}

	private void CreateTopShakerTab()
	{
		DbLine dbLine;
		int GrowId;
		int  id;
		Point SplitPt;
		Point SplitPt2;
		int gary;
		double ActualPT;
		Point ActualStartPT;
		Point ActualEndPT;


		Viewer.RefreshView();
		Selector.Flush();

		ShakerTab = new DbTool("Shaker_Tab");
		ShakerTab = Model.ToolGet("Shaker_Tab");
		ShakerTab.Color( DbEntity.RGB(141, 179, 226) );

		TopLN=Model.EntityGet("TopLine");

		TopName = DbLine.DbLine( TopLN);

		TempStartPt = TopName.StartPt();
		TempMidPt = TopName.MidPt();
		TempEndPt = TopName.EndPt();

		TempStartX = TempStartPt.X();
		TempStartY = TempMidPt.Y();


		TempPT = new Point(TempMidPt.X() - splitdist,TempStartY, 0);

		if (RightMidPt.X() > 98)
		{

				SplitPt = new Point((TempStartPt.X() + 20) - (splitdist/2), TempStartY,0);

				if((matwidth - TempMidPt.Y()) > .125)
				{
					ActualStartPT = new Point(TempStartPt.X() + splitdist,TempStartPt.Y(),0.0);
					ActualEndPT = new Point((TempStartPt.X() + 20) - (splitdist/2), TempStartY,0);
					dbLine = new DbLine(ActualStartPT,ActualEndPT);
					TempID =  Portal.IntGet("id");
					dbLine.Color( DbEntity.RGB(141, 179, 226) );
//
//					Portal.Execute("toolpath:autoindexoffset: toolid=" + Rect2_90X125.Id() + ",profid=" + TempID + ",dir =1,longside = 1");
//					id = Portal.IntGet("id");
//
//					T_Trim1 = new DbFeature();
//					T_Trim1.Id( id );
//					T_Trim1.Name("Top_Trim1");


					Portal.Execute("toolpath:autoindexoffset: toolid=" +  Rect2_90X125.Id() + ",profid=" + TempID + ",dir =1,longside = 1");
					int top_trim1ID = Portal.IntGet("id");
					Portal.Execute("*Feature:Entity:id=" + top_trim1ID + ",index=0");
					top_trim1ID=Portal.IntGet("id");
					Portal.Execute("Entity:Name: id=" + top_trim1ID + ",name= Top_Trim1");

					Selector.Flush();

					Selector.All(true);
					Selector.Add(Model.EntityGet("Top_Trim1"));
					Portal.Execute("Feature:Modify: id=" + Zone1ID + ",selected=1");

					SplitPt2 = new Point((TempStartPt.X() + 20) + (splitdist/2), TempStartY,0);
					ActualStartPT = new Point((TempStartPt.X() + 20) + (splitdist/2), TempStartY,0);
					ActualEndPT = new Point(TempMidPt.X() - (splitdist/2),TempStartY, 0);


					dbLine = new DbLine(ActualStartPT, ActualEndPT);
					TempID =  Portal.IntGet("id");
					dbLine.Color( DbEntity.RGB(141, 179, 226) );

					Portal.Execute("toolpath:autoindexoffset: toolid=" + Rect2_90X125.Id() + ",profid=" + TempID + ",dir =1,longside = 1");
					id = Portal.IntGet("id");

					T_Trim2 = new DbFeature();
					T_Trim2.Id( id );
					T_Trim2.Name("Top_Trim2");

					Selector.Flush();

					Selector.All(true);
					Selector.Add(Model.EntityGet("Top_Trim2"));
					Portal.Execute("Feature:Modify: id=" + Zone2ID + ",selected=1");
//

					TempPT = new Point(TempMidPt.X() + splitdist,TempStartY, 0);

					ActualStartPT = new Point(TempMidPt.X() + (splitdist/2),TempStartY, 0);;
					ActualEndPT = new Point(TempEndPt.X() - splitdist,TempStartPt.Y(),0.0);

					dbLine = new DbLine(ActualStartPT, ActualEndPT);
					TempID =  Portal.IntGet("id");
					dbLine.Color( DbEntity.RGB(141, 179, 226) );

					Portal.Execute("toolpath:autoindexoffset: toolid=" + Rect2_90X125.Id() + ",profid=" + TempID + ",dir =1,longside = 1");
					id = Portal.IntGet("id");

					T_Trim3 = new DbFeature();
					T_Trim3.Id( id );
					T_Trim3.Name("Top_Trim3");

					Selector.Flush();

					Selector.All(true);
					Selector.Add(Model.EntityGet("Top_Trim3"));
					//Msg.Display("We should be assigning it");
					Portal.Execute("Feature:Modify: id=" + Zone2ID + ",selected=1");

				}
				else
				{

					ActualStartPT = new Point(TempStartPt.X() + splitdist,TempStartPt.Y(),0.0);
					ActualEndPT = new Point((TempStartPt.X() + 20) - (splitdist/2), TempStartY,0);

					dbLine = new DbLine(ActualStartPT,ActualEndPT);
					TempID =  Portal.IntGet("id");
					dbLine.Color( DbEntity.RGB(141, 179, 226) );

					Portal.Execute("toolpath:autoindexoffset: toolid=" + Sqr1000.Id() + ",profid=" + TempID + ",dir =1,longside = 1");
					id = Portal.IntGet("id");

					T_Trim1 = new DbFeature();
					T_Trim1.Id( id );
					T_Trim1.Name("Top_Trim1");

					//Msg.Display("Here #2");

					Selector.Flush();

					Selector.All(true);
					Selector.Add(Model.EntityGet("Top_Trim1"));
					Portal.Execute("Feature:Modify: id=" + Zone1ID + ",selected=1");

					SplitPt2 = new Point((TempStartPt.X() + 20) + (splitdist/2), TempStartY,0);


					ActualStartPT = new Point((TempStartPt.X() + 20) + (splitdist/2), TempStartY,0);
					ActualEndPT = new Point(TempMidPt.X() - (splitdist/2),TempStartY, 0);

					dbLine = new DbLine(ActualStartPT, ActualEndPT);
					TempID =  Portal.IntGet("id");
					dbLine.Color( DbEntity.RGB(141, 179, 226) );

					Portal.Execute("toolpath:autoindexoffset: toolid=" + Sqr1000.Id() + ",profid=" + TempID + ",dir =1,longside = 1");
					id = Portal.IntGet("id");

					T_Trim2 = new DbFeature();
					T_Trim2.Id( id );
					T_Trim2.Name("Top_Trim2");

					Selector.Flush();

					Selector.All(true);
					Selector.Add(Model.EntityGet("Top_Trim2"));
					Portal.Execute("Feature:Modify: id=" + Zone2ID + ",selected=1");


					ActualStartPT = new Point(TempMidPt.X() + (splitdist/2),TempStartY, 0);
					ActualEndPT = new Point(TempEndPt.X() - splitdist,TempStartPt.Y(),0.0);

					dbLine = new DbLine(ActualStartPT, ActualEndPT);
					//dbLine = new DbLine(TempPT, TempEndPt);
					TempID =  Portal.IntGet("id");
					dbLine.Color( DbEntity.RGB(141, 179, 226) );

					Portal.Execute("toolpath:autoindexoffset: toolid=" + Sqr1000.Id() + ",profid=" + TempID + ",dir =1,longside = 1");
					id = Portal.IntGet("id");

					T_Trim3 = new DbFeature();
					T_Trim3.Id( id );
					T_Trim3.Name("Top_Trim3");

					Selector.Flush();

					Selector.All(true);
					Selector.Add(Model.EntityGet("Top_Trim3"));

					//Msg.Display("Did we assign it");
					Portal.Execute("Feature:Modify: id=" + Zone2ID + ",selected=1");
					Selector.Flush();

				}
		}
		else
		{

			SplitPt2 = new Point((TempStartPt.X() + 20) + splitdist, TempStartY,0);
			ActualEndPT = new Point(TempMidPt.X() - (splitdist/2),TempStartY, 0);

			ActualStartPT = new Point(TempStartPt.X() + splitdist,TempStartPt.Y(),0.0);
			//dbLine = new DbLine(ActualStartPT,SplitPt);

			dbLine = new DbLine(ActualStartPT, ActualEndPT);
			TempID =  Portal.IntGet("id");

			dbLine.Color( DbEntity.RGB(141, 179, 226) );

			if((matwidth - TempMidPt.Y()) > .125)
			{
				Portal.Execute("toolpath:autoindexoffset: toolid=" + Rect2_90X125.Id() + ",profid=" + TempID + ",dir =1,longside = 1");
				id = Portal.IntGet("id");

				T_Trim1 = new DbFeature();
				T_Trim1.Id( id );
				T_Trim1.Name("Top_Trim1");

				//Msg.Display("Here #3");

				Selector.Flush();

				Selector.All(true);
				Selector.Add(Model.EntityGet("Top_Trim1"));
				Portal.Execute("Feature:Modify: id=" + Zone1ID + ",selected=1");
				Selector.Flush();


			}
			else
			{
				Portal.Execute("toolpath:autoindexoffset: toolid=" + Sqr1000.Id() + ",profid=" + TempID + ",dir =1,longside = 1");
				id = Portal.IntGet("id");

				T_Trim1 = new DbFeature();
				T_Trim1.Id( id );
				T_Trim1.Name("Top_Trim1");

				//Msg.Display("Here #4");

				Selector.Flush();

				Selector.All(true);
				Selector.Add(Model.EntityGet("Top_Trim1"));
				Portal.Execute("Feature:Modify: id=" + Zone1ID + ",selected=1");
				Selector.Flush();


			}

			TempPT = new Point(TempMidPt.X() + (splitdist/2),TempStartY, 0);

			ActualEndPT = new Point(TempEndPt.X() - splitdist,TempStartPt.Y(),0.0);

			dbLine = new DbLine(TempPT, ActualEndPT);
			TempID =  Portal.IntGet("id");
			//dbLine = new DbLine(TempPT, TempEndPt);

			dbLine.Color( DbEntity.RGB(141, 179, 226) );

			if((matwidth - TempMidPt.Y()) > .125)
			{
				Portal.Execute("toolpath:autoindexoffset: toolid=" + Rect2_90X125.Id() + ",profid=" + TempID + ",dir =1,longside = 1");
				id = Portal.IntGet("id");

				T_Trim2 = new DbFeature();
				T_Trim2.Id( id );
				T_Trim2.Name("Top_Trim2");

				Selector.Flush();

				Selector.All(true);
				Selector.Add(Model.EntityGet("Top_Trim2"));
				Portal.Execute("Feature:Modify: id=" + Zone1ID + ",selected=1");
				Selector.Flush();


			}
			else
			{
				Portal.Execute("toolpath:autoindexoffset: toolid=" + Sqr1000.Id() + ",profid=" + TempID + ",dir =1,longside = 1");
				id = Portal.IntGet("id");

				T_Trim2 = new DbFeature();
				T_Trim2.Id( id );
				T_Trim2.Name("Top_Trim2");

				Selector.Flush();

				Selector.All(true);
				Selector.Add(Model.EntityGet("Top_Trim2"));
				Portal.Execute("Feature:Modify: id=" + Zone1ID + ",selected=1");
				Selector.Flush();


			}

		}
	}

	private void CreateOldTopShakerTab()
	{
		DbLine dbLine;
		int GrowId;
		int  id;
		Point SplitPt;
		Point SplitPt2;
		int gary;


		Viewer.RefreshView();
		Selector.Flush();

		ShakerTab = new DbTool("Shaker_Tab");
		ShakerTab = Model.ToolGet("Shaker_Tab");
		ShakerTab.Color( DbEntity.RGB(141, 179, 226) );

		TopLN=Model.EntityGet("TopLine");

		TopName = DbLine.DbLine( TopLN);

		TempStartPt = TopName.StartPt();
		TempMidPt = TopName.MidPt();
		TempEndPt = TopName.EndPt();

		TempStartX = TempStartPt.X();
		TempStartY = TempMidPt.Y();


		TempPT = new Point(TempMidPt.X() - splitdist,TempStartY, 0);

		if (RightMidPt.X() > 98)
		{
			//Msg.Display("The RepoAmt should be < " + (TempStartPt.X() + 20) + " >");

				SplitPt = new Point((TempStartPt.X() + 20) - splitdist, TempStartY,0);

				if((matwidth - TempMidPt.Y()) > .125)
				{
					dbLine = new DbLine(TempStartPt,SplitPt);
					TempID =  Portal.IntGet("id");
					dbLine.Color( DbEntity.RGB(141, 179, 226) );

					Portal.Execute("toolpath:autoindexoffset: toolid=" + Rect2_90X125.Id() + ",profid=" + TempID + ",dir =1,longside = 1");
					id = Portal.IntGet("id");

					T_Trim1 = new DbFeature();
					T_Trim1.Id( id );
					T_Trim1.Name("Top_Trim1");

					Selector.Flush();

					Selector.All(true);
					Selector.Add(Model.EntityGet("Top_Trim1"));
					Portal.Execute("Feature:Modify: id=" + Zone1ID + ",selected=1");

					SplitPt2 = new Point((TempStartPt.X() + 20) + splitdist, TempStartY,0);


					dbLine = new DbLine(SplitPt2, TempPT);
					TempID =  Portal.IntGet("id");
					dbLine.Color( DbEntity.RGB(141, 179, 226) );

					Portal.Execute("toolpath:autoindexoffset: toolid=" + Rect2_90X125.Id() + ",profid=" + TempID + ",dir =1,longside = 1");
					id = Portal.IntGet("id");

					T_Trim2 = new DbFeature();
					T_Trim2.Id( id );
					T_Trim2.Name("Top_Trim2");

					Selector.Flush();

					Selector.All(true);
					Selector.Add(Model.EntityGet("Top_Trim2"));
					Portal.Execute("Feature:Modify: id=" + Zone2ID + ",selected=1");


					TempPT = new Point(TempMidPt.X() + splitdist,TempStartY, 0);

					dbLine = new DbLine(TempPT, TempEndPt);
					TempID =  Portal.IntGet("id");
					dbLine.Color( DbEntity.RGB(141, 179, 226) );

					Portal.Execute("toolpath:autoindexoffset: toolid=" + Rect2_90X125.Id() + ",profid=" + TempID + ",dir =1,longside = 1");
					id = Portal.IntGet("id");

					T_Trim3 = new DbFeature();
					T_Trim3.Id( id );
					T_Trim3.Name("Top_Trim3");

					Selector.Flush();

					Selector.All(true);
					Selector.Add(Model.EntityGet("Top_Trim3"));
					Portal.Execute("Feature:Modify: id=" + Zone2ID + ",selected=1");

				}
				else
				{

					dbLine = new DbLine(TempStartPt,SplitPt);
					TempID =  Portal.IntGet("id");
					dbLine.Color( DbEntity.RGB(141, 179, 226) );

					Portal.Execute("toolpath:autoindexoffset: toolid=" + Sqr1000.Id() + ",profid=" + TempID + ",dir =1,longside = 1");
					id = Portal.IntGet("id");

					T_Trim1 = new DbFeature();
					T_Trim1.Id( id );
					T_Trim1.Name("Top_Trim1");

					Selector.Flush();

					Selector.All(true);
					Selector.Add(Model.EntityGet("Top_Trim1"));
					Portal.Execute("Feature:Modify: id=" + Zone1ID + ",selected=1");

					SplitPt2 = new Point((TempStartPt.X() + 20) + splitdist, TempStartY,0);


					dbLine = new DbLine(SplitPt2, TempPT);
					TempID =  Portal.IntGet("id");
					dbLine.Color( DbEntity.RGB(141, 179, 226) );

					Portal.Execute("toolpath:autoindexoffset: toolid=" + Sqr1000.Id() + ",profid=" + TempID + ",dir =1,longside = 1");
					id = Portal.IntGet("id");

					T_Trim2 = new DbFeature();
					T_Trim2.Id( id );
					T_Trim2.Name("Top_Trim2");

					Selector.Flush();

					Selector.All(true);
					Selector.Add(Model.EntityGet("Top_Trim2"));
					Portal.Execute("Feature:Modify: id=" + Zone2ID + ",selected=1");


					TempPT = new Point(TempMidPt.X() + splitdist,TempStartY, 0);

					dbLine = new DbLine(TempPT, TempEndPt);
					TempID =  Portal.IntGet("id");
					dbLine.Color( DbEntity.RGB(141, 179, 226) );

					Portal.Execute("toolpath:autoindexoffset: toolid=" + Sqr1000.Id() + ",profid=" + TempID + ",dir =1,longside = 1");
					id = Portal.IntGet("id");

					T_Trim3 = new DbFeature();
					T_Trim3.Id( id );
					T_Trim3.Name("Top_Trim3");

					Selector.Flush();

					Selector.All(true);
					Selector.Add(Model.EntityGet("Top_Trim3"));
					Portal.Execute("Feature:Modify: id=" + Zone2ID + ",selected=1");

				}
		}
		else
		{

			SplitPt2 = new Point((TempStartPt.X() + 20) + splitdist, TempStartY,0);
			TempPT = new Point(TempMidPt.X() - splitdist,TempStartY, 0);

			dbLine = new DbLine(TempStartPt, TempPT);
			TempID =  Portal.IntGet("id");
			dbLine.Color( DbEntity.RGB(141, 179, 226) );

			if((matwidth - TempMidPt.Y()) > .125)
			{
				Portal.Execute("toolpath:autoindexoffset: toolid=" + Rect2_90X125.Id() + ",profid=" + TempID + ",dir =1,longside = 1");
				id = Portal.IntGet("id");

				T_Trim1 = new DbFeature();
				T_Trim1.Id( id );
				T_Trim1.Name("Top_Trim1");

				Selector.Flush();

				Selector.All(true);
				Selector.Add(Model.EntityGet("Top_Trim1"));
				Portal.Execute("Feature:Modify: id=" + Zone1ID + ",selected=1");


			}
			else
			{
				Portal.Execute("toolpath:autoindexoffset: toolid=" + Sqr1000.Id() + ",profid=" + TempID + ",dir =1,longside = 1");
				id = Portal.IntGet("id");

				T_Trim1 = new DbFeature();
				T_Trim1.Id( id );
				T_Trim1.Name("Top_Trim1");

				Selector.Flush();

				Selector.All(true);
				Selector.Add(Model.EntityGet("Top_Trim1"));
				Portal.Execute("Feature:Modify: id=" + Zone1ID + ",selected=1");


			}

			TempPT = new Point(TempMidPt.X() + splitdist,TempStartY, 0);

			dbLine = new DbLine(TempPT, TempEndPt);
			TempID =  Portal.IntGet("id");
			dbLine.Color( DbEntity.RGB(141, 179, 226) );

			if((matwidth - TempMidPt.Y()) > .125)
			{
				Portal.Execute("toolpath:autoindexoffset: toolid=" + Rect2_90X125.Id() + ",profid=" + TempID + ",dir =1,longside = 1");
				id = Portal.IntGet("id");

				T_Trim2 = new DbFeature();
				T_Trim2.Id( id );
				T_Trim2.Name("Top_Trim2");

				Selector.Flush();

				Selector.All(true);
				Selector.Add(Model.EntityGet("Top_Trim2"));
				Portal.Execute("Feature:Modify: id=" + Zone1ID + ",selected=1");


			}
			else
			{
				Portal.Execute("toolpath:autoindexoffset: toolid=" + Sqr1000.Id() + ",profid=" + TempID + ",dir =1,longside = 1");
				id = Portal.IntGet("id");

				T_Trim2 = new DbFeature();
				T_Trim2.Id( id );
				T_Trim2.Name("Top_Trim2");

				Selector.Flush();

				Selector.All(true);
				Selector.Add(Model.EntityGet("Top_Trim2"));
				Portal.Execute("Feature:Modify: id=" + Zone1ID + ",selected=1");


			}

		}
	}

//	private void CreateTopShakerTab()
//	{
//		DbLine dbLine;
//		int GrowId;
//		int  id;
//		Point SplitPt;
//		Point SplitPt2;
//		int gary;
//
//
//		Viewer.RefreshView();
//		Selector.Flush();
//
//
//		ShakerTab = new DbTool("Shaker_Tab");
//		ShakerTab = Model.ToolGet("Shaker_Tab");
//		ShakerTab.Color( DbEntity.RGB(141, 179, 226) );
//
//		TopLN=Model.EntityGet("TopLine");
//
//		TopName = DbLine.DbLine( TopLN);
//
//		TempStartPt = TopName.StartPt();
//		TempMidPt = TopName.MidPt();
//		TempEndPt = TopName.EndPt();
//
//		TempStartX = TempStartPt.X();
//		TempStartY = TempMidPt.Y();
//		SplitPt = new Point((TempStartPt.X() + 20) - splitdist, TempStartY,0);
//
//		TempPT = new Point(TempMidPt.X() - splitdist,TempStartY, 0);
//
//		if (RightMidPt.X() > 98)
//		{
//		//	Msg.Display("Top Skaer Lines The width of the panel is < " + proflen + " > and the X for the Split is < " + SplitPt.X() +" >");
//			dbLine = new DbLine(TempStartPt,SplitPt);
//			TempID= dbLine.Id();
//			dbLine.Color( DbEntity.RGB(141, 179, 226) );
//
//			if((matwidth - TempMidPt.Y()) > .125)
//				{
//					Portal.Execute("toolpath:autoindexoffset: toolid=" + Rect2_90X125.Id() + ",profid=" + TempID + ",dir =1,longside = 1");
//					id = Portal.IntGet("id");
//
//					T_Trim1 = new DbFeature();
//					T_Trim1.Id( id );
//					T_Trim1.Name("T_Trim1");
//				}
//			else
//			{
//					Portal.Execute("toolpath:autoindexoffset: toolid=" + Sqr1000.Id() + ",profid=" + TempID + ",dir =1,longside = 1");
//			}
//
//			SplitPt2 = new Point((TempStartPt.X() + 20) + splitdist, TempStartY,0);
//
//
//			dbLine = new DbLine(SplitPt2, TempPT);
//			dbLine.Color( DbEntity.RGB(141, 179, 226) );
//
//			if((matwidth - TempMidPt.Y()) > .125)
//				{
//					Portal.Execute("toolpath:autoindexoffset: toolid=" + Rect2_90X125.Id() + ",profid=" + TempID + ",dir =1,longside = 1");
//
//
//				}
//			else
//			{
//					Portal.Execute("toolpath:autoindexoffset: toolid=" + Sqr1000.Id() + ",profid=" + TempID + ",dir =1,longside = 1");
//			}
//		else
//
//		{
//
//			TempPT = new Point(TempMidPt.X() - splitdist,TempStartY, 0);
//
//			dbLine = new DbLine(TempStartPt, TempPT);
//			dbLine.Color( DbEntity.RGB(141, 179, 226) );
//
//			GrowId= dbLine.Id();
//
//		}
//
//		GrowId= dbLine.Id();
//
//		TempPT = new Point(TempMidPt.X() + splitdist,TempStartY, 0);
//
//		dbLine = new DbLine(TempPT, TempEndPt);
//		dbLine.Color( DbEntity.RGB(141, 179, 226) );
//
////		Portal.Execute("profile:grow: seed=" + GrowId + ",tol=.1,clean=0,assoc=0,same=0,special=0");
////		TempID = Portal.IntGet("id");
////
////		Portal.Execute("*selector:select:id=" + TempID);
//
//		if((matwidth - TempMidPt.Y()) > .125)
//		{
////			Portal.Execute("toolpath:autoindexoffset: toolid=" + Rect2_90X125.Id() + ",profid=" + TempID + ",dir =1,longside = 1");
////
////			id = Portal.IntGet("id");
////			Portal.Execute("*entity:type: id=" + id);
////
////			TopTrim = new DbFeature();
////			TopTrim.Id( id );
////			TopTrim.Name("TopTrim");
////			gary = TopTrim.Count();
////Msg.Display("Hello The first top count is < " + gary + " >");
//
////			if(TopTrim.Count() > 2)
////			{
////				T_Trim1 = TopTrim.Get(0);
////				T_Trim1.Name("T_Trim3");
////				T_Trim2 = TopTrim.Get(1);
////				T_Trim2.Name("T_Trim1");
////				T_Trim3 = TopTrim.Get(2);
////				T_Trim3.Name("T_Trim2");
////			}
////			else
////			{
////				T_Trim1 = TopTrim.Get(0);
////				T_Trim1.Name("T_Trim2");
////				T_Trim2 = TopTrim.Get(1);
////				T_Trim2.Name("T_Trim1");
////
////			}
//
//
//		}
////		else
////		{
////			Portal.Execute("toolpath:autoindexoffset: toolid=" + Sqr1000.Id() + ",profid=" + TempID + ",dir =1,longside = 1");
////
////			id = Portal.IntGet("id");
////			Portal.Execute("*entity:type: id=" + id);
////
////			TopTrim = new DbFeature();
////			TopTrim.Id( id );
////			TopTrim.Name("TopTrim");
////			gary = TopTrim.Count();
//////			T_Trim1 = TopTrim.Get(0);
//////			T_Trim1.Name("T_Trim1");
//////			T_Trim2 = TopTrim.Get(1);
//////			T_Trim2.Name("T_Trim2");
//////			T_Trim3 = TopTrim.Get(2);
//////			T_Trim3.Name("T_Trim3");
////
////
////		}
//
//
//	}
//

	private void CreateRightShakerTab()
	{
          DbLine dbLine;
		int GrowId;
		int  id;
		double ActualPT;
		Point ActualStartPT;
		Point ActualEndPT;


		Viewer.RefreshView();
		Selector.Flush();


		ShakerTab = new DbTool("Shaker_Tab");
		ShakerTab = Model.ToolGet("Shaker_Tab");
		ShakerTab.Color( DbEntity.RGB(141, 179, 226) );

		RightLN=Model.EntityGet("RightLine");

		RightName = DbLine.DbLine( RightLN);

		TempStartPt = RightName.StartPt();
		TempMidPt = RightName.MidPt();
		TempEndPt = RightName.EndPt();

		ActualPT =TempStartPt.Y() - (splitdist);

		ActualStartPT = new Point(TempStartPt.X(),ActualPT,0.0);
		ActualEndPT =  new Point(TempEndPt.X(),TempMidPt.Y() + (splitdist/2), 0.0);

		dbLine = new DbLine(ActualStartPT, ActualEndPT);

		TempID = Portal.IntGet("id");

		dbLine.Color( DbEntity.RGB(141, 179, 226) );

		if((matlength - TempMidPt.X()) > .125)
		{
			if(DEBUG)Msg.Display("CreateRightShakerTab 1");

			Portal.Execute("toolpath:autoindexoffset: toolid=" + Rect2_90X125.Id() + ",profid=" + TempID + ",dir =1,longside = 1");
			id = Portal.IntGet("id");

			R_Trim1 = new DbFeature();
			R_Trim1.Id( id );
			R_Trim1.Name("R_Trim1");

			Selector.Flush();

			Selector.All(true);
			Selector.Add(Model.EntityGet("R_Trim1"));

			if (RightMidPt.X() > 98)
			{
				if(DEBUG)Msg.Display("CreateRightShakerTab 2");

				Portal.Execute("Feature:Modify: id=" + Zone2ID + ",selected=1");
			}
			else
			{

				if(CreatedZone2)
				{
					if(DEBUG)Msg.Display("CreateRightShakerTab 3");

					Portal.Execute("Feature:Modify: id=" + Zone2ID + ",selected=1");
				}
				else
				{
					if(DEBUG)Msg.Display("CreateRightShakerTab 4");

					Portal.Execute("Feature:Modify: id=" + Zone1ID + ",selected=1");
				}
			}
			Selector.Flush();

		}
		else
		{
			//Portal.Execute("toolpath:autoindexoffset: toolid=" + Sqr1000.Id() + ",profid=" + TempID + ",dir =1,longside = 1");
			Portal.Execute("toolpath:autoindexoffset: toolid=" + Rect2_90X125.Id() + ",profid=" + TempID + ",dir =1,longside = 1");
			id = Portal.IntGet("id");

			R_Trim1 = new DbFeature();
			R_Trim1.Id( id );
			R_Trim1.Name("R_Trim1");

			Selector.Flush();

			Selector.All(true);
			Selector.Add(Model.EntityGet("R_Trim1"));

//			if (RightMidPt.X() > 98)
//			{
//				if(TESTING)Msg.Display("CreateRightShakerTab 5");
//
//				Portal.Execute("Feature:Modify: id=" + Zone2ID + ",selected=1");
//			}
//			else
//			{
				if(DEBUG)Msg.Display("CreateRightShakerTab 6");

					Portal.Execute("Feature:Modify: id=" + Zone2ID + ",selected=1");


			//}

			Selector.Flush();

		}

		ActualStartPT = new Point(TempStartPt.X(),TempMidPt.Y()-(splitdist/2),0.0);
		ActualEndPT =  new Point(TempEndPt.X(),TempEndPt.Y() - splitdist, 0.0);


		dbLine = new DbLine(ActualStartPT, ActualEndPT);
		TempID = Portal.IntGet("id");
		dbLine.Color( DbEntity.RGB(141, 179, 226) );

		if((matlength - TempMidPt.X()) > .125)
		{
			Portal.Execute("toolpath:autoindexoffset: toolid=" + Rect2_90X125.Id() + ",profid=" + TempID + ",dir =1,longside = 1");
			id = Portal.IntGet("id");

			R_Trim2 = new DbFeature();
			R_Trim2.Id( id );
			R_Trim2.Name("R_Trim2");

			Selector.Flush();

			Selector.All(true);
			Selector.Add(Model.EntityGet("R_Trim2"));

			if (RightMidPt.X() > 98)
			{
				if(DEBUG)Msg.Display("CreateRightShakerTab 7");

				Portal.Execute("Feature:Modify: id=" + Zone2ID + ",selected=1");
			}
			else
			{
				if(CreatedZone2)
				{
					if(DEBUG)Msg.Display("CreateRightShakerTab 8");
					//Msg.Display("Here");
					Portal.Execute("Feature:Modify: id=" + Zone2ID + ",selected=1");
				}
				else
				{
					if(DEBUG)Msg.Display("CreateRightShakerTab 9");

					Portal.Execute("Feature:Modify: id=" + Zone1ID + ",selected=1");
				}
			}
			Selector.Flush();

		}
		else
		{
			Portal.Execute("toolpath:autoindexoffset: toolid=" + Rect2_90X125.Id() + ",profid=" + TempID + ",dir =1,longside = 1");
			id = Portal.IntGet("id");

			R_Trim2 = new DbFeature();
			R_Trim2.Id( id );
			R_Trim2.Name("R_Trim2");

			Selector.Flush();

			Selector.All(true);
			Selector.Add(Model.EntityGet("R_Trim2"));

//			if (RightMidPt.X() > 98)
//			{
//				if(TESTING)Msg.Display("CreateRightShakerTab 10");
//
//				Portal.Execute("Feature:Modify: id=" + Zone2ID + ",selected=1");
//			}
//			else
//			{
				if(DEBUG)Msg.Display("CreateRightShakerTab 11");

				Portal.Execute("Feature:Modify: id=" + Zone2ID + ",selected=1");
			//}

			Selector.Flush();
		}

	}

	private void CreateOldRightShakerTab()
	{
          DbLine dbLine;
		int GrowId;
		int  id;

		Viewer.RefreshView();
		Selector.Flush();


		ShakerTab = new DbTool("Shaker_Tab");
		ShakerTab = Model.ToolGet("Shaker_Tab");
		ShakerTab.Color( DbEntity.RGB(141, 179, 226) );

		RightLN=Model.EntityGet("RightLine");

		RightName = DbLine.DbLine( RightLN);

		TempStartPt = RightName.StartPt();
		TempMidPt = RightName.MidPt();
		TempEndPt = RightName.EndPt();

		TempStartX = TempStartPt.X();


		TempStartY = TempMidPt.Y() + splitdist;
		//Msg.Display("the X is < " + TempStartPt.X() + " > and the endYis < " + TempStartY + " > ");

		TempPT = new Point(TempStartX,TempStartY, 0);

		dbLine = new DbLine(TempStartPt, TempPT);
		TempID = Portal.IntGet("id");

		dbLine.Color( DbEntity.RGB(141, 179, 226) );

		if((matlength - TempMidPt.X()) > .125)
		{
			Portal.Execute("toolpath:autoindexoffset: toolid=" + Rect2_90X125.Id() + ",profid=" + TempID + ",dir =1,longside = 1");
			id = Portal.IntGet("id");

			R_Trim1 = new DbFeature();
			R_Trim1.Id( id );
			R_Trim1.Name("R_Trim1");

			Selector.Flush();

			Selector.All(true);
			Selector.Add(Model.EntityGet("R_Trim1"));

			if (RightMidPt.X() > 98)
			{

				Portal.Execute("Feature:Modify: id=" + Zone2ID + ",selected=1");
			}
			else
			{

				Portal.Execute("Feature:Modify: id=" + Zone1ID + ",selected=1");
			}

		}
		else
		{
			Portal.Execute("toolpath:autoindexoffset: toolid=" + Sqr1000.Id() + ",profid=" + TempID + ",dir =1,longside = 1");
			id = Portal.IntGet("id");

			R_Trim1 = new DbFeature();
			R_Trim1.Id( id );
			R_Trim1.Name("R_Trim1");

			Selector.Flush();

			Selector.All(true);
			Selector.Add(Model.EntityGet("R_Trim1"));

			if (RightMidPt.X() > 98)
			{

				Portal.Execute("Feature:Modify: id=" + Zone2ID + ",selected=1");
			}
			else
			{

				Portal.Execute("Feature:Modify: id=" + Zone1ID + ",selected=1");
			}



		}

		TempStartY = TempMidPt.Y() - splitdist;
		TempPT = new Point(TempStartX,TempStartY, 0);

		dbLine = new DbLine(TempPT, TempEndPt);
		TempID = Portal.IntGet("id");
		dbLine.Color( DbEntity.RGB(141, 179, 226) );

		if((matlength - TempMidPt.X()) > .125)
		{
			Portal.Execute("toolpath:autoindexoffset: toolid=" + Rect2_90X125.Id() + ",profid=" + TempID + ",dir =1,longside = 1");
			id = Portal.IntGet("id");

			R_Trim2 = new DbFeature();
			R_Trim2.Id( id );
			R_Trim2.Name("R_Trim2");

			Selector.Flush();

			Selector.All(true);
			Selector.Add(Model.EntityGet("R_Trim2"));

			if (RightMidPt.X() > 98)
			{

				Portal.Execute("Feature:Modify: id=" + Zone2ID + ",selected=1");
			}
			else
			{

				Portal.Execute("Feature:Modify: id=" + Zone1ID + ",selected=1");
			}

		}
		else
		{
			Portal.Execute("toolpath:autoindexoffset: toolid=" + Sqr1000.Id() + ",profid=" + TempID + ",dir =1,longside = 1");
			id = Portal.IntGet("id");

			R_Trim2 = new DbFeature();
			R_Trim2.Id( id );
			R_Trim2.Name("R_Trim2");

			Selector.Flush();

			Selector.All(true);
			Selector.Add(Model.EntityGet("R_Trim2"));

			if (RightMidPt.X() > 98)
			{

				Portal.Execute("Feature:Modify: id=" + Zone2ID + ",selected=1");
			}
			else
			{

				Portal.Execute("Feature:Modify: id=" + Zone1ID + ",selected=1");
			}


		}

	}



	private void CreateBottomShakerTab()
	{
		DbLine dbLine;
		int GrowId;
		int  id;
		Point SplitPt;
		Point SplitPt2;
		double ActualPT;
		Point ActualStartPT;
		Point ActualEndPT;



		Viewer.RefreshView();
		Selector.Flush();


		ShakerTab = new DbTool("Shaker_Tab");
		ShakerTab = Model.ToolGet("Shaker_Tab");
		ShakerTab.Color( DbEntity.RGB(141, 179, 226) );

		BottomLN=Model.EntityGet("BottomLine");

		BottomName = DbLine.DbLine( BottomLN);

		TempStartPt = BottomName.StartPt();
		TempMidPt = BottomName.MidPt();
		TempEndPt = BottomName.EndPt();

		TempStartX = TempStartPt.X();
		TempStartY = TempMidPt.Y();

		SplitPt = new Point((TempMidPt.X() ) + (splitdist/2), TempStartY,0);

		TempPT = new Point(TempMidPt.X() - (splitdist/2),TempStartY, 0);
		ActualPT =TempStartPt.X() - (splitdist);
		ActualStartPT = new Point(TempStartPt.X() - (splitdist),TempMidPt.Y(),0.0);
		ActualEndPT =  new Point(TempEndPt.X()  + .250+ splitdist,TempMidPt.Y(), 0.0 );

		dbLine = new DbLine(ActualStartPT,SplitPt);
		TempID =  Portal.IntGet("id");
		dbLine.Color( DbEntity.RGB(141, 179, 226) );
		if (RightMidPt.X() > 98)
		{

			if((matlength - TempMidPt.X()) > .125)
			{
				Portal.Execute("toolpath:autoindexoffset: toolid=" + Rect2_90X125.Id() + ",profid=" + TempID + ",dir =1,longside = 1");
				id = Portal.IntGet("id");


			}
			else
			{
				Portal.Execute("toolpath:autoindexoffset: toolid=" + Sqr1000.Id() + ",profid=" + TempID + ",dir =1,longside = 1");
				id = Portal.IntGet("id");

			}

			B_Trim1 = new DbFeature();
			B_Trim1.Id( id );
			B_Trim1.Name("B_Trim1");

			Selector.Flush();

			Selector.All(true);
			Selector.Add(Model.EntityGet("B_Trim1"));
			Portal.Execute("Feature:Modify: id=" + Zone2ID + ",selected=1");


			ActualEndPT = new Point((TempEndPt.X() ) + (splitdist/2) + 20, TempEndPt.Y(),0);

			dbLine = new DbLine(TempPT, ActualEndPT);
			TempID =  Portal.IntGet("id");
			dbLine.Color( DbEntity.RGB(141, 179, 226) );

			if((matlength - TempMidPt.X()) > .125)
			{
				Portal.Execute("toolpath:autoindexoffset: toolid=" + Rect2_90X125.Id() + ",profid=" + TempID + ",dir =1,longside = 1");
				id = Portal.IntGet("id");


			}
			else
			{
				Portal.Execute("toolpath:autoindexoffset: toolid=" + Sqr1000.Id() + ",profid=" + TempID + ",dir =1,longside = 1");
				id = Portal.IntGet("id");

			}

			B_Trim2 = new DbFeature();
			B_Trim2.Id( id );
			B_Trim2.Name("B_Trim2");

			Selector.Flush();

			Selector.All(true);
			Selector.Add(Model.EntityGet("B_Trim2"));


			Portal.Execute("Feature:Modify: id=" + Zone2ID + ",selected=1");

			TempPT = new Point(TempEndPt.X() - (splitdist/2) + 20,TempStartY, 0);
			ActualPT =TempEndPt.X() + (splitdist/2);

			ActualEndPT =  new Point(ActualPT,TempMidPt.Y(), 0.0 );
			dbLine = new DbLine(TempPT, ActualEndPT);
			TempID =  Portal.IntGet("id");
			dbLine.Color( DbEntity.RGB(141, 179, 226) );

			if((matlength - TempMidPt.X()) > .125)
			{
				Portal.Execute("toolpath:autoindexoffset: toolid=" + Rect2_90X125.Id() + ",profid=" + TempID + ",dir =1,longside = 1");
				id = Portal.IntGet("id");


			}
			else
			{
				Portal.Execute("toolpath:autoindexoffset: toolid=" + Sqr1000.Id() + ",profid=" + TempID + ",dir =1,longside = 1");
				id = Portal.IntGet("id");

			}

			B_Trim3 = new DbFeature();
			B_Trim3.Id( id );
			B_Trim3.Name("B_Trim3");

			Selector.Flush();

			Selector.All(true);
			Selector.Add(Model.EntityGet("B_Trim3"));


			Portal.Execute("Feature:Modify: id=" + Zone1ID + ",selected=1");
		}
		else
		{

				if((matlength - TempMidPt.X()) > .125)
				{
					Portal.Execute("toolpath:autoindexoffset: toolid=" + Rect2_90X125.Id() + ",profid=" + TempID + ",dir =1,longside = 1");
					id = Portal.IntGet("id");


				}
				else
				{
					Portal.Execute("toolpath:autoindexoffset: toolid=" + Sqr1000.Id() + ",profid=" + TempID + ",dir =1,longside = 1");
					id = Portal.IntGet("id");

				}

				B_Trim1 = new DbFeature();
				B_Trim1.Id( id );
				B_Trim1.Name("B_Trim1");

				Selector.Flush();

				Selector.All(true);
				Selector.Add(Model.EntityGet("B_Trim1"));
				Portal.Execute("Feature:Modify: id=" + Zone1ID + ",selected=1");


				ActualStartPT = new Point(TempMidPt.X()- (splitdist/2), TempMidPt.Y(),0.0);
				ActualEndPT = new Point(TempEndPt.X()+ splitdist, TempMidPt.Y(),0.0);

				dbLine = new DbLine(ActualStartPT, ActualEndPT);
				TempID =  Portal.IntGet("id");
				dbLine.Color( DbEntity.RGB(141, 179, 226) );

				if((matlength - TempMidPt.X()) > .125)
				{
					Portal.Execute("toolpath:autoindexoffset: toolid=" + Rect2_90X125.Id() + ",profid=" + TempID + ",dir =1,longside = 1");
					id = Portal.IntGet("id");


				}
				else
				{
					Portal.Execute("toolpath:autoindexoffset: toolid=" + Sqr1000.Id() + ",profid=" + TempID + ",dir =1,longside = 1");
					id = Portal.IntGet("id");

				}

				B_Trim2 = new DbFeature();
				B_Trim2.Id( id );
				B_Trim2.Name("B_Trim2");

				Selector.Flush();

				Selector.All(true);
				Selector.Add(Model.EntityGet("B_Trim2"));
				Portal.Execute("Feature:Modify: id=" + Zone1ID + ",selected=1");
		}
	}

	private void CreatelOldBottomShakerTab()
	{
		DbLine dbLine;
		int GrowId;
		int  id;
		Point SplitPt;
		Point SplitPt2;

		Viewer.RefreshView();
		Selector.Flush();


		ShakerTab = new DbTool("Shaker_Tab");
		ShakerTab = Model.ToolGet("Shaker_Tab");
		ShakerTab.Color( DbEntity.RGB(141, 179, 226) );

		BottomLN=Model.EntityGet("BottomLine");

		BottomName = DbLine.DbLine( BottomLN);

		TempStartPt = BottomName.StartPt();
		TempMidPt = BottomName.MidPt();
		TempEndPt = BottomName.EndPt();

		TempStartX = TempStartPt.X();
		TempStartY = TempMidPt.Y();

		SplitPt = new Point((TempMidPt.X() ) + splitdist, TempStartY,0);

		TempPT = new Point(TempMidPt.X() - splitdist,TempStartY, 0);

		dbLine = new DbLine(TempStartPt,SplitPt);
		TempID =  Portal.IntGet("id");
		dbLine.Color( DbEntity.RGB(141, 179, 226) );
		if (RightMidPt.X() > 98)
		{

			if((matlength - TempMidPt.X()) > .125)
			{
				Portal.Execute("toolpath:autoindexoffset: toolid=" + Rect2_90X125.Id() + ",profid=" + TempID + ",dir =1,longside = 1");
				id = Portal.IntGet("id");


			}
			else
			{
				Portal.Execute("toolpath:autoindexoffset: toolid=" + Sqr1000.Id() + ",profid=" + TempID + ",dir =1,longside = 1");
				id = Portal.IntGet("id");

			}

			B_Trim1 = new DbFeature();
			B_Trim1.Id( id );
			B_Trim1.Name("B_Trim1");

			Selector.Flush();

			Selector.All(true);
			Selector.Add(Model.EntityGet("B_Trim1"));
			Portal.Execute("Feature:Modify: id=" + Zone2ID + ",selected=1");


			SplitPt2 = new Point((TempEndPt.X() ) + splitdist + 20, TempEndPt.Y(),0);

			dbLine = new DbLine(TempPT, SplitPt2);
			TempID =  Portal.IntGet("id");
			dbLine.Color( DbEntity.RGB(141, 179, 226) );

			if((matlength - TempMidPt.X()) > .125)
			{
				Portal.Execute("toolpath:autoindexoffset: toolid=" + Rect2_90X125.Id() + ",profid=" + TempID + ",dir =1,longside = 1");
				id = Portal.IntGet("id");


			}
			else
			{
				Portal.Execute("toolpath:autoindexoffset: toolid=" + Sqr1000.Id() + ",profid=" + TempID + ",dir =1,longside = 1");
				id = Portal.IntGet("id");

			}

			B_Trim2 = new DbFeature();
			B_Trim2.Id( id );
			B_Trim2.Name("B_Trim2");

			Selector.Flush();

			Selector.All(true);
			Selector.Add(Model.EntityGet("B_Trim2"));


			Portal.Execute("Feature:Modify: id=" + Zone2ID + ",selected=1");

			TempPT = new Point(TempEndPt.X() - splitdist + 20,TempStartY, 0);

			dbLine = new DbLine(TempPT, TempEndPt);
			TempID =  Portal.IntGet("id");
			dbLine.Color( DbEntity.RGB(141, 179, 226) );

			if((matlength - TempMidPt.X()) > .125)
			{
				Portal.Execute("toolpath:autoindexoffset: toolid=" + Rect2_90X125.Id() + ",profid=" + TempID + ",dir =1,longside = 1");
				id = Portal.IntGet("id");


			}
			else
			{
				Portal.Execute("toolpath:autoindexoffset: toolid=" + Sqr1000.Id() + ",profid=" + TempID + ",dir =1,longside = 1");
				id = Portal.IntGet("id");

			}

			B_Trim3 = new DbFeature();
			B_Trim3.Id( id );
			B_Trim3.Name("B_Trim3");

			Selector.Flush();

			Selector.All(true);
			Selector.Add(Model.EntityGet("B_Trim3"));


			Portal.Execute("Feature:Modify: id=" + Zone1ID + ",selected=1");
		}
		else
		{

			BottomLN=Model.EntityGet("BottomLine");

			BottomName = DbLine.DbLine( BottomLN);

			TempStartPt = BottomName.StartPt();
			TempMidPt = BottomName.MidPt();
			TempEndPt = BottomName.EndPt();

			TempPT = new Point(TempMidPt.X() - splitdist,TempStartY, 0);


			SplitPt2 = new Point((SplitPt.X()) - splitdist, TempStartY,0);
			SplitPt2 = new Point((TempMidPt.X() ) + splitdist, TempEndPt.Y(),0);

			dbLine = new DbLine(TempStartPt, SplitPt2);
			dbLine.Color( DbEntity.RGB(141, 179, 226) );


			BottomLN=Model.EntityGet("BottomLine");

			BottomName = DbLine.DbLine( BottomLN);

			TempStartPt = BottomName.StartPt();
			TempMidPt = BottomName.MidPt();
			TempEndPt = BottomName.EndPt();

			TempPT = new Point(TempMidPt.X() - splitdist,TempStartY, 0);

			SplitPt2 = new Point((TempMidPt.X() ) + splitdist, TempEndPt.Y(),0);

			dbLine = new DbLine(TempStartPt, SplitPt2);
			TempID =  Portal.IntGet("id");
			dbLine.Color( DbEntity.RGB(141, 179, 226) );

			if((matlength - TempMidPt.X()) > .125)
			{
				Portal.Execute("toolpath:autoindexoffset: toolid=" + Rect2_90X125.Id() + ",profid=" + TempID + ",dir =1,longside = 1");
				id = Portal.IntGet("id");


			}
			else
			{
				Portal.Execute("toolpath:autoindexoffset: toolid=" + Sqr1000.Id() + ",profid=" + TempID + ",dir =1,longside = 1");
				id = Portal.IntGet("id");

			}

			B_Trim1 = new DbFeature();
			B_Trim1.Id( id );
			B_Trim1.Name("B_Trim1");

			Selector.Flush();

			Selector.All(true);
			Selector.Add(Model.EntityGet("B_Trim1"));
			Portal.Execute("Feature:Modify: id=" + Zone1ID + ",selected=1");


			TempPT = new Point(TempMidPt.X() - splitdist,TempStartY, 0);

			//Msg.Display(" The TempPT x  is < " + TempPT.X() + " > and the X for the TempEndPt is < " + TempEndPt.X() + " > ");

			SplitPt2 = new Point((TempMidPt.X() ) - splitdist, TempEndPt.Y(),0);

			dbLine = new DbLine(SplitPt2, TempEndPt);
			TempID =  Portal.IntGet("id");
			dbLine.Color( DbEntity.RGB(141, 179, 226) );

			if((matlength - TempMidPt.X()) > .125)
			{
				Portal.Execute("toolpath:autoindexoffset: toolid=" + Rect2_90X125.Id() + ",profid=" + TempID + ",dir =1,longside = 1");
				id = Portal.IntGet("id");


			}
			else
			{
				Portal.Execute("toolpath:autoindexoffset: toolid=" + Sqr1000.Id() + ",profid=" + TempID + ",dir =1,longside = 1");
				id = Portal.IntGet("id");

			}

			B_Trim2 = new DbFeature();
			B_Trim2.Id( id );
			B_Trim2.Name("B_Trim2");

			Selector.Flush();

			Selector.All(true);
			Selector.Add(Model.EntityGet("B_Trim2"));
			Portal.Execute("Feature:Modify: id=" + Zone1ID + ",selected=1");

		}


//		if (RightMidPt.X() > 98)
//		{
//
//			//Msg.Display(" The TempStartPt x  is < " + TempStartPt.X() + " > and the X for the Split is < " + SplitPt.X() +" >");
//
//			dbLine = new DbLine(TempStartPt,SplitPt);
//			TempID =  Portal.IntGet("id");
//			dbLine.Color( DbEntity.RGB(141, 179, 226) );
//
//			BottomLN=Model.EntityGet("BottomLine");
//
//			BottomName = DbLine.DbLine( BottomLN);
//
//			TempStartPt = BottomName.StartPt();
//			TempMidPt = BottomName.MidPt();
//			TempEndPt = BottomName.EndPt();
//
//			TempPT = new Point(TempMidPt.X() - splitdist,TempStartY, 0);
//
//
//			SplitPt2 = new Point((SplitPt.X()) - splitdist, TempStartY,0);
//			SplitPt2 = new Point((TempEndPt.X() ) + splitdist + 20, TempEndPt.Y(),0);
//
//			//Msg.Display(" The TempPT x  is < " + TempPT.X() + " > and the X for the Split2 is < " + SplitPt2.X() + " > ");
//
//
//			dbLine = new DbLine(TempPT, SplitPt2);
//			TempID =  Portal.IntGet("id");
//			dbLine.Color( DbEntity.RGB(141, 179, 226) );
//
//			GrowId= dbLine.Id();
//
//			TempPT = new Point(TempEndPt.X() - splitdist + 20,TempStartY, 0);
//
//			dbLine = new DbLine(TempPT, TempEndPt);
//			TempID =  Portal.IntGet("id");
//			dbLine.Color( DbEntity.RGB(141, 179, 226) );
//
//			Portal.Execute("profile:grow: seed=" + GrowId + ",tol=.1,clean=0,assoc=0,same=0,special=0");
//			TempID = Portal.IntGet("id");
//
//			Portal.Execute("*selector:select:id=" + TempID);
//
//			Portal.Execute("toolpath:autoindexoffset: toolid=" + Rect2_90X125.Id() + ",profid=" + TempID + ",dir =1,longside = 1");
//
//
//			id = Portal.IntGet("id");
//			Portal.Execute("*entity:type: id=" + id);
//
//			BottomTrim = new DbFeature();
//			BottomTrim.Id( id );
//			BottomTrim.Name("Bottom_Trim");
//
//			if(BottomTrim.Count() > 2)
//			{
//				B_Trim1 = BottomTrim.Get(0);
//				B_Trim1.Name("B_Trim3");
//				B_Trim2 = BottomTrim.Get(1);
//				B_Trim2.Name("B_Trim1");
//				B_Trim3 = BottomTrim.Get(2);
//				B_Trim3.Name("B_Trim2");
//			}
//			else
//			{
//				B_Trim1 = BottomTrim.Get(0);
//				B_Trim1.Name("B_Trim2");
//				B_Trim2 = BottomTrim.Get(1);
//				B_Trim2.Name("B_Trim1");
//
//			}
//
//
//		}
//		else
//		{
//
//			BottomLN=Model.EntityGet("BottomLine");
//
//			BottomName = DbLine.DbLine( BottomLN);
//
//			TempStartPt = BottomName.StartPt();
//			TempMidPt = BottomName.MidPt();
//			TempEndPt = BottomName.EndPt();
//
//			TempPT = new Point(TempMidPt.X() - splitdist,TempStartY, 0);
//
//
//			SplitPt2 = new Point((SplitPt.X()) - splitdist, TempStartY,0);
//			SplitPt2 = new Point((TempMidPt.X() ) + splitdist, TempEndPt.Y(),0);
//
//			dbLine = new DbLine(TempStartPt, SplitPt2);
//			TempID =  Portal.IntGet("id");
//			dbLine.Color( DbEntity.RGB(141, 179, 226) );
//
//			GrowId= dbLine.Id();
//
//			TempPT = new Point(TempMidPt.X() - splitdist,TempStartY, 0);
//
//			//Msg.Display(" The TempPT x  is < " + TempPT.X() + " > and the X for the TempEndPt is < " + TempEndPt.X() + " > ");
//
//			SplitPt2 = new Point((TempMidPt.X() ) - splitdist, TempEndPt.Y(),0);
//
//			dbLine = new DbLine(SplitPt2, TempEndPt);
//			TempID =  Portal.IntGet("id");
//			dbLine.Color( DbEntity.RGB(141, 179, 226) );
//
//			GrowId= dbLine.Id();
//
//			Portal.Execute("profile:grow: seed=" + GrowId + ",tol=.1,clean=0,assoc=0,same=0,special=0");
//			TempID = Portal.IntGet("id");
//
//			Portal.Execute("*selector:select:id=" + TempID);
//
//			Portal.Execute("toolpath:autoindexoffset: toolid=" + Rect2_90X125.Id() + ",profid=" + TempID + ",dir =1,longside = 1");
//
//			int  id;
//			id = Portal.IntGet("id");
//			Portal.Execute("*entity:type: id=" + id);
//
//			BottomTrim = new DbFeature();
//			BottomTrim.Id( id );
//			BottomTrim.Name("Bottom_Trim");
//
//			if(BottomTrim.Count() > 2)
//			{
//				B_Trim1 = BottomTrim.Get(0);
//				B_Trim1.Name("B_Trim3");
//				B_Trim2 = BottomTrim.Get(1);
//				B_Trim2.Name("B_Trim1");
//				B_Trim3 = BottomTrim.Get(2);
//				B_Trim3.Name("B_Trim2");
//			}
//			else
//			{
//				B_Trim1 = BottomTrim.Get(0);
//				B_Trim1.Name("B_Trim2");
//				B_Trim2 = BottomTrim.Get(1);
//				B_Trim2.Name("B_Trim1");
//
//			}
//
//		}


	}

	private void CreateShakerTabs()
	{

		CreateLeftShakerTab();

		CreateTopShakerTab();

		CreateRightShakerTab();

		//if(DoesClearDeadZone > DeadZoneWid + DeadZoneBuffer)
		//{
			CreateBottomShakerTab();
		//}

	}

	private void SetClamps()
	{


		if(matlength > 73)
		{
			clamp2 = 17;
			Model.StringSet("Clamp2Pos","79.918890");
			//79.918890
		}

		double pos2 = (startclamp  * clamp2);

	}
	private void CheckUnderClamps()
	{
		DbArc	dbArc;
		DbLine	dbLine;
		DbEntity dbEntity = null;
		Point	ps;
		Point	pe;
		Point	pc;
		double xmin, ymin, xmax,ymax;
		int type;
		DbHole		dbHole;
		int myID;

	    Selector.Flush();
		Selector.All( false );
		Selector.Hole( true);


        xmin = GetFirstClampLeft();
        ymin = 0;
        xmax = GetFirstClampLeft() + DeadZoneLen;
        ymax = 2.8642;

		Selector.Add( Model.EntityGet("Sqr1000_Hole1"));
		Selector.Add( Model.EntityGet("Sqr1000_Hole2"));
		Selector.Add( Model.EntityGet("Sqr1000_Hole3"));
		Selector.Add( Model.EntityGet("Sqr1000_Hole4"));

    	int count,indx,id;

    	count = Selector.Count();

    	for (indx = count-1; indx >= 0; --indx)
		{
			dbEntity = Selector.Get(indx);


			type = dbEntity.Type();

			if(dbEntity != null);

			{
				dbHole = DbHole.DbHole( Selector.Get(indx) );

				if (dbHole != null)
				{
					if( type == Const.HOLE)
					{
						dbHole = DbHole.DbHole(dbEntity);

						myID = dbHole.Id();

						if (dbHole != null)
						{

							pc = dbHole.CenterPt();

							if(pc.X() > xmin && pc.Y() >ymin && pc.X() < xmax && pc.Y() < ymax)
							{
								//Msg.Display("Removing the hit");

								Selector.Remove(dbEntity);
							}

						}
					}

				}

			}
		}

	}

	private void HideLayers()
	{
		DbTool	dbTool;
		int		count, indx;

		count = Model.EntityCount( Const.TOOL );

		if (DEBUG) Msg.Diagnostic("~~~~~~~~~~~~~~~~~~~~~~~~~~The tool count is " + count );

		for (indx = 0; indx < count; ++indx)
		{
			dbTool = Model.ToolGet( indx );

			if ( dbTool.IsLayer() )
			{
				Portal.Execute("entity:hide:id=" + dbTool.Id());
				continue;
			}

		}
	}

		private void ShowLayers()
	{
		DbTool	dbTool;
		int		count, indx;

		count = Model.EntityCount( Const.TOOL );

		if (DEBUG) Msg.Diagnostic("~~~~~~~~~~~~~~~~~~~~~~~~~~The tool count is " + count );

		for (indx = 0; indx < count; ++indx)
		{
			dbTool = Model.ToolGet( indx );

			if ( dbTool.IsLayer() )
			{
				Portal.Execute("entity:show:id=" + dbTool.Id());
				continue;
			}

		}
	}

	private void SelctBox(double xmin, double ymin, double xmax, double ymax)
	{
		String cmd;

		Selector.Flush();
		Selector.All( true );


	        cmd = "selector:selectbox:"
	      + " xmin=" + xmin
	      + ",ymin=" + ymin
	      + ",zmin=0"
	      + ",xmax=" + xmax
	      + ",ymax=" + ymax
	      + ",zmax=0";

	      Portal.Execute(cmd);

	}

	private void HoleClampCheck( DbHole dTheHole, double xe )
	{
		String fname;
		int indx;
		int myzone = 1;
		double delta = 0 - repoLocation;

		double myxe = xe;
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

///#$###$#$##$###$#$#$##$#$#$##$#$#$#$#$##

	public void ClusterStuff()
	{
		int prof = Model.IntGet( "mac.prof" );
		double length;
		double width;

		if(Holes_W_Notch())
		{
			 length = proflen + 2.25; //Model.DoubleGet("mac.mylength") + 2.25;// was 2.5
			 width = profwid + 2.25; //Model.DoubleGet("mac.mywidth") + 2.25;// was 2.5

		}
		else
		{
			 length  =proflen; // Model.DoubleGet("mac.mylength");// was 2.5
			 width = profwid; // Model.DoubleGet("mac.mywidth");// was 2.5
		}


		int left = Model.IntGet( "mac.left" );
		int right = Model.IntGet( "mac.right" );
		int top = Model.IntGet( "mac.top" );
		int bottom = Model.IntGet( "mac.bottom" );
		final double TOL = 0.2;
		DbHole		dbHole = null;
		DbTool dbToolA = Model.ToolByStation(945871710);//( 945871710 )(1020);
		DbTool dbToolB = Model.ToolByStation(10265);//( 10265 )(1016);
		DbTool dbToolC = Model.ToolByStation(4737032);//( 10265 )(1016);
		DbTool dbToolD = Model.ToolByStation(410000200);//( 10265 )(1016);
		DbTool dbToolE = Model.ToolByStation(41000);//( 10265 )(1016);
		DbTool dbToolF = Model.ToolByStation(40373);//( 10265 )(1016);

		if(prof == 1)
		{

				dbHole = new DbHole( -.2363, 1.8455, 0., 1.e-1, 0. );
				dbHole.Tool( dbToolE );
				dbHole.DoubleSet( "orient", 39.79  );
				dbHole = new DbHole( length + .2363, 1.8455, 0., 1.e-1, 0. );
				dbHole.Tool( dbToolE );
				dbHole.DoubleSet( "orient", 50.27  );
				dbHole = new DbHole( length + .2363, width - 1.8455, 0., 1.e-1, 0. );
				dbHole.Tool( dbToolE );
				dbHole.DoubleSet( "orient", -50.27  );
				dbHole = new DbHole( -0.2363, width - 1.8455, 0., 1.e-1, 0. );
				dbHole.Tool( dbToolE );
				dbHole.DoubleSet( "orient", -39.79  );


				dbHole = new DbHole(1.8455,-.2363, 0., 1.e-1, 0. );
				dbHole.Tool( dbToolE );
				dbHole.DoubleSet( "orient", 50.27  );
				dbHole = new DbHole( length - 1.8455, -.2363, 0., 1.e-1, 0. );
				dbHole.Tool( dbToolE );
				dbHole.DoubleSet( "orient", 39.79  );
				dbHole = new DbHole( length -1.8455, width +.2363, 0., 1.e-1, 0. );
				dbHole.Tool( dbToolE );
				dbHole.DoubleSet( "orient", -39.79  );
				dbHole = new DbHole( 1.8455, width +.2363, 0., 1.e-1, 0. );
				dbHole.Tool( dbToolE );
				dbHole.DoubleSet( "orient", -50.27  );


				dbHole = new DbHole( 0.290, 0.290, 0., 1.e-1, 0. );
				dbHole.Tool( dbToolE );
				dbHole.DoubleSet( "orient", 0.0  );
				dbHole = new DbHole( 0.290, width - 0.290, 0., 1.e-1, 0. );
				dbHole.Tool( dbToolE );
				dbHole.DoubleSet( "orient", 0.0  );
				dbHole = new DbHole( length - 0.290, 0.290, 0., 1.e-1, 0. );
				dbHole.Tool( dbToolE );
				dbHole.DoubleSet( "orient", 0.0  );
				dbHole = new DbHole( length - 0.290, width - 0.290, 0., 1.e-1, 0. );
				dbHole.Tool( dbToolE );
				dbHole.DoubleSet( "orient", 0.0  );


				dbHole = new DbHole( -.032, 1.28, 0., 1.e-1, 0. );
				dbHole.Tool( dbToolE );
				dbHole.DoubleSet( "orient", 0.0  );
				dbHole = new DbHole(-.032, width - 1.28, 0., 1.e-1, 0. );
				dbHole.Tool( dbToolE );
				dbHole.DoubleSet( "orient", 0.0  );
				dbHole = new DbHole( length +.032, 1.28, 0., 1.e-1, 0. );
				dbHole.Tool( dbToolE );
				dbHole.DoubleSet( "orient", 0.0  );
				dbHole = new DbHole( length +.032, width - 1.28, 0., 1.e-1, 0. );
				dbHole.Tool( dbToolE );
				dbHole.DoubleSet( "orient", 0.0  );

				dbHole = new DbHole(1.28, -.032, 0., 1.e-1, 0. );
				dbHole.Tool( dbToolE );
				dbHole.DoubleSet( "orient", 0.0  );
				dbHole = new DbHole(1.28, width +.032, 0., 1.e-1, 0. );
				dbHole.Tool( dbToolE );
				dbHole.DoubleSet( "orient", 0.0  );
				dbHole = new DbHole( length -1.28, -.032, 0., 1.e-1, 0. );
				dbHole.Tool( dbToolE );
				dbHole.DoubleSet( "orient", 0.0  );
				dbHole = new DbHole( length -1.28, width +.032, 0., 1.e-1, 0. );
				dbHole.Tool( dbToolE );
				dbHole.DoubleSet( "orient", 0.0  );

				dbHole = new DbHole(0.656, 0.656, 0., 1.e-1, 0. );
				dbHole.Tool( dbToolE );
				dbHole.DoubleSet( "orient", 0.0  );
				dbHole = new DbHole(0.656, width - 0.656, 0., 1.e-1, 0. );
				dbHole.Tool( dbToolE );
				dbHole.DoubleSet( "orient", 0.0  );
				dbHole = new DbHole( length -0.656, 0.656, 0., 1.e-1, 0. );
				dbHole.Tool( dbToolE );
				dbHole.DoubleSet( "orient", 0.0  );
				dbHole = new DbHole( length -0.656, width - 0.656, 0., 1.e-1, 0. );
				dbHole.Tool( dbToolE );
				dbHole.DoubleSet( "orient", 0.0  );




		}

					DbFeature dbFeature;
					DbLine	cut;
					double orient;


		double myLength;
		double myWidth;

		if(Holes_W_Notch() )
		{

			myLength = (length - 2.3);//was 2.8
			myWidth = (width - 2.3);//was 2.8

		}
		else
		{


			myLength = (length - .5);
			myWidth = (width - .5);


		}
		int count, indx, type, refCount, Hit, indx2;

		double newlength = Math.floor(myLength);

		double newwidth = Math.floor(myWidth);
		Msg.DiagnosticsEnable( DEBUG );

		int ovalshort = VarSys.IntGet( "mac.ovalshort" );
		int ovalon = VarSys.IntGet( "mac.ovalon" );

		DbTool		dbTool;
		dbTool = Model.ToolGet("Layer_1");

		int leven = 0;
		int lodd = 0;
		int wleven = 0;
		int wlodd = 0;
		int wodd = 0;
		int weven = 0;
		int wodd2 = 0;
		int lwodd = 0;
		int lweven = 0;
		int lwodd2 = 0;
		int wodd3 = 0;


		double clusterHits = (Math.floor(myLength)) / 2;
		double wclusterHits = (Math.floor(myWidth)) / 3;
		double myclusterHits = Math.floor((Math.floor(myLength)) / 2);
		double sumyclusterHits = Math.floor((Math.floor(myLength)) / 3);
		double wmyclusterHits = Math.floor((Math.floor(myWidth)) / 3);
		double suwmyclusterHits = Math.floor((Math.floor(myWidth)) / 2);
		double wotherHits = wclusterHits - wmyclusterHits;
		double wcount = 0.5;
		double otherHits = clusterHits - myclusterHits;
		double myotherhits = (Math.floor(myLength)) / 3;

		if(otherHits> .0)myclusterHits = myclusterHits - 1;

		double count2 = 0.666;
		double count1 = 0.333;
		double secondhit = (length/2) + (newlength/2);
		double tol;
		double firstWhit = (width/2) - (newwidth/2) + 1.5;
		double offset = 1.0;
			if ( Math.abs(wotherHits-count1) <= TOL  )
			{
				wmyclusterHits = wmyclusterHits - 1;
				suwmyclusterHits = suwmyclusterHits -2;

			}

			if (myotherhits - sumyclusterHits > 0.5  )
			{
				 myotherhits = 1;
			}
			else
			{
				 myotherhits = 0;
			}


	 if (Math.floor(myLength) % 2 == 0)
	 {
	 		leven = 1;
	 }
	 else
	 {
	 		lodd = 1;
	 }

	 if (Math.floor(myWidth) % 2 == 0)
	 {
	 		wleven = 1;
	 }
	 else
	 {
//	 		Msg.Display("we are here!!!! diffH =" );
	 		wlodd = 1;
	 }

	 if (Math.floor(myWidth) % 3 == 0)
	 {
	 		weven = 1;
//	 		Msg.Display("we are here!!!! width = even =" );
	 }
	 else
	 {
	 	if(wotherHits > 0.5)
	 	{
	  		wodd = 1;
	 	}
	 	else
	 	{
	 		wodd2 = 1;
	 	}
	 }

	 if (Math.floor(myLength) % 3 == 0)
	 {
	 		lweven = 1;
//	 		Msg.Display("we are here!!!! width = even =" );
	 }
	 else
	 {
//	 		Msg.Display("we are here!!!! width = otherhits ="  + myotherhits );
	 	if(myotherhits == 0)
	 	{
	  		lwodd = 1;
	 	}
	 	else
	 	{
	 		lwodd2 = 1;
	 	}
	 }

int diffH = 0;

if(lodd == 1 & wodd == 1 & wleven == 1 & lwodd2 == 1)
{
	diffH = 3;
	suwmyclusterHits = suwmyclusterHits - 1;
//	Msg.Display("we are here!!!! diffH =" + diffH );
}
if(lodd == 1 & wodd == 1 & wlodd == 1 & lweven == 1)
{
	diffH = 4;
	suwmyclusterHits = suwmyclusterHits - 1;
//	Msg.Display("we are here!!!! diffH =" + diffH );
}
if(leven == 1 & wodd == 1 & wleven == 1 & lwodd2 == 1)
{
	diffH = 3;
//	Msg.Display("we are here!!!! diffH =" + diffH );
}
if(leven == 1 & wodd == 1 & wlodd == 1 & lwodd2 == 1)
{
	diffH = 3;
//	Msg.Display("we are here!!!! diffH =" + diffH );
}
if(lodd == 1 & wodd == 1 & wleven == 1 & lwodd == 1)
{
	diffH = 2;
	suwmyclusterHits = suwmyclusterHits - 1;
//	Msg.Display("we are here!!!! diffH =" + diffH );
}
if(lodd == 1 & wodd == 1 & wlodd == 1 & lwodd2 == 1)
{
	diffH = 10;
	suwmyclusterHits = suwmyclusterHits - 1;
//	Msg.Display("we are here!!!! diffH =" + diffH );
}
if(lodd == 1 & wodd == 1 & wlodd == 1 & lwodd == 1)
{
	diffH = 11;
	suwmyclusterHits = suwmyclusterHits - 1;
//	Msg.Display("we are here!!!! diffH =" + diffH );
}
if(lodd == 1 & wodd2 == 1& wlodd ==1  &  lweven == 1)
{
	diffH = 4;
//	Msg.Display("we are here!!!! diffH =" + diffH );
}
if(lodd == 1 & weven == 1& wlodd ==1  &  lweven == 1)
{
	diffH = 4;
//	Msg.Display("we are here!!!! diffH =" + diffH );
}
if(lodd == 1 & weven == 1& wlodd ==1  &  lwodd2 == 1)
{
	diffH = 4;
//	Msg.Display("we are here!!!! diffH =" + diffH );
}
if(lodd == 1 & weven == 1& wlodd ==1  &  lwodd == 1)
{
	diffH = 4;
//	Msg.Display("we are here!!!! diffH =" + diffH );
}
if(lodd == 1 & wodd2 == 1& lwodd ==1  & wlodd == 1)
{
	diffH = 8;
//	Msg.Display("we are here!!!! diffH =" + diffH );
}
if(lodd == 1 & wodd2 == 1& wlodd ==1  & lwodd2 == 1)
{
	diffH = 7;
//	Msg.Display("we are here!!!! diffH =" + diffH );
}
if(leven == 1 & wodd2 == 1& wlodd ==1  & lwodd2 == 1)
{
	diffH = 9;
//	Msg.Display("we are here!!!! diffH =" + diffH );
}
if(leven == 1 & wodd2 == 1& wleven ==1  & lwodd2 == 1)
{
	diffH = 9;
//	Msg.Display("we are here!!!! diffH =" + diffH );
}
if(lodd == 1 & wodd2 == 1& wleven ==1  & lwodd2 == 1)
{
	diffH = 9;
//	Msg.Display("we are here!!!! diffH =" + diffH );
}
if(leven == 1 & wodd == 1 & wleven == 1 & lwodd == 1)
{
	diffH = 5;
//	Msg.Display("we are here!!!! diffH =" + diffH );
}
if(leven == 1 & wodd == 1 & wlodd == 1 & lwodd == 1)
{
	diffH = 5;
//	Msg.Display("we are here!!!! diffH =" + diffH );
}
if(leven == 1 & wodd2 == 1& wlodd ==1  & lwodd == 1)
{
	diffH = 6;
//	Msg.Display("we are here!!!! diffH =" + diffH );
}
if(lodd == 1 & wodd2 == 1& wleven ==1  & lwodd == 1)
{
	diffH = 6;
//	Msg.Display("we are here!!!! diffH =" + diffH );
}
if(leven == 1 & wodd2 == 1& wleven ==1  & lwodd == 1)
{
	diffH = 6;
//	Msg.Display("we are here!!!! diffH =" + diffH );
}

		double SUfirstWhit = firstWhit;
			double firsthit = 0;

		for (indx = 0; indx < wmyclusterHits; ++indx)
		{

			tol = .01;


			firsthit = ((length/2) - (newlength/2)) + offset;
			for (indx2 = 0; indx2 < myclusterHits; ++indx2)
			{
				dbHole = new DbHole( firsthit, firstWhit, 0., 1.e-1, 0. );
				dbHole.IntSet( "PressureFoot", 1 );
				orient = 90;
				dbHole.DoubleSet("orient", orient);
				dbHole.Tool(dbToolA);
				firsthit = firsthit + 2.0;

			}
			firstWhit = firstWhit + 3.0;




		}


//---------------------------------------------------------------------------------------------------------------------------------------

		for (indx = 0; indx < suwmyclusterHits; ++indx)
		{

			if ( Math.abs(count1 - otherHits) <= TOL  )
			{

				dbHole = new DbHole( secondhit-1.5 , SUfirstWhit-.5, 0., 1.e-1, 0. );
				dbHole.IntSet( "PressureFoot", 1 );
				orient = 00;
				dbHole.DoubleSet("orient", orient);
				dbHole.Tool(dbToolA);

//		Msg.Display("we are here!!!! " + (secondhit - 1.5) );
			}
			SUfirstWhit = SUfirstWhit + 2.0;
		}


		double oddfirst = (length/2) - (newlength/2)+.5;

		if ( Math.abs(wcount - wotherHits) <= TOL  )
		{

//		Msg.Display("Math.abs(count2 - otherHits==) "  + Math.abs(count2 - otherHits));
			firstWhit = firstWhit- 1.0;
//		if (test == 4) sumyclusterHits = sumyclusterHits-1 ;

			for (indx2 = 0; indx2 < sumyclusterHits; ++indx2)
			{
//							Msg.Display("count2= "  + count2 + "outerhits:" + otherHits);
				dbHole = new DbHole( oddfirst +1.0, firstWhit + .5, 0., 1.e-1, 0. );
				dbHole.Tool(dbToolA);
				dbHole.IntSet( "PressureFoot", 1 );
				orient = 00;
				dbHole.DoubleSet("orient", orient);

				if ( Math.abs(count1 - wotherHits) <= TOL  )
				{
					dbHole = new DbHole(  oddfirst+1.0, firstWhit +2.5, 0., 1.e-1, 0. );
					dbHole.Tool(dbToolA);
					dbHole.IntSet( "PressureFoot", 1 );
					orient = 00;
					dbHole.DoubleSet("orient", orient);
//					Msg.Display("count2= "  + (oddfirst + 1.0));
				}
				oddfirst = oddfirst + 3.0;
			}

		}
		if(diffH == 9 )

		{
			dbHole = new DbHole( secondhit-1,firstWhit+1 , 0., 1.e-1, 0. );
			dbHole.IntSet( "PressureFoot", 1 );
			orient = 90;
			dbHole.DoubleSet("orient", orient);
			dbHole.Tool(dbToolA);

			dbHole = new DbHole( secondhit-1.5, firstWhit+3.0, 0., 1.e-1, 0. );
			dbHole.Tool(dbToolB);
			dbHole = new DbHole( secondhit-0.5,firstWhit+3.0 , 0., 1.e-1, 0. );
			dbHole.Tool(dbToolB);
		}
		if(diffH == 7 )

		{
			dbHole = new DbHole( secondhit-1,firstWhit+1 , 0., 1.e-1, 0. );
			dbHole.IntSet( "PressureFoot", 1 );
			orient = 90;
			dbHole.DoubleSet("orient", orient);
			dbHole.Tool(dbToolA);

			dbHole = new DbHole( secondhit-1.5, firstWhit+3.0, 0., 1.e-1, 0. );
			dbHole.Tool(dbToolB);
			dbHole = new DbHole( secondhit-0.5,firstWhit+3.0 , 0., 1.e-1, 0. );
			dbHole.Tool(dbToolB);

			dbHole = new DbHole( secondhit-2.5,SUfirstWhit-1 , 0., 1.e-1, 0. );
			dbHole.Tool(dbToolB);
			dbHole = new DbHole( secondhit-1.5,SUfirstWhit-1 , 0., 1.e-1, 0. );
			dbHole.Tool(dbToolB);
			dbHole = new DbHole( secondhit-.5,SUfirstWhit-1 , 0., 1.e-1, 0. );
			dbHole.Tool(dbToolB);
		}
		if(diffH == 4 )
		{
			dbHole = new DbHole( secondhit-2.5,SUfirstWhit-1 , 0., 1.e-1, 0. );
			dbHole.Tool(dbToolB);
			dbHole = new DbHole( secondhit-1.5,SUfirstWhit-1 , 0., 1.e-1, 0. );
			dbHole.Tool(dbToolB);
			dbHole = new DbHole( secondhit-.5,SUfirstWhit-1 , 0., 1.e-1, 0. );
			dbHole.Tool(dbToolB);
		}
		if(diffH == 11 )
		{
			dbHole = new DbHole( secondhit-2.5,SUfirstWhit-1 , 0., 1.e-1, 0. );
			dbHole.Tool(dbToolB);
			dbHole = new DbHole( secondhit-1.5,SUfirstWhit-1 , 0., 1.e-1, 0. );
			dbHole.Tool(dbToolB);
			dbHole = new DbHole( secondhit-.5,SUfirstWhit-1 , 0., 1.e-1, 0. );
			dbHole.Tool(dbToolB);
			dbHole = new DbHole( secondhit-.5,firstWhit+1 , 0., 1.e-1, 0. );
			dbHole.Tool(dbToolB);
			dbHole = new DbHole( secondhit-.5,firstWhit , 0., 1.e-1, 0. );
			dbHole.Tool(dbToolB);
		}
		if (diffH == 6 )
		{
			dbHole = new DbHole( secondhit-.5,firstWhit+3 , 0., 1.e-1, 0. );
			dbHole.Tool(dbToolB);
			dbHole = new DbHole( secondhit-.5,firstWhit+2 , 0., 1.e-1, 0. );
			dbHole.Tool(dbToolB);
			dbHole = new DbHole( secondhit-.5,firstWhit+1 , 0., 1.e-1, 0. );
			dbHole.Tool(dbToolB);
			dbHole = new DbHole( secondhit-.5,firstWhit , 0., 1.e-1, 0. );
			dbHole.Tool(dbToolB);

		}

		if (diffH == 5 )
		{
			dbHole = new DbHole( secondhit-.5,firstWhit+1 , 0., 1.e-1, 0. );
			dbHole.Tool(dbToolB);
			dbHole = new DbHole( secondhit-.5,firstWhit , 0., 1.e-1, 0. );
			dbHole.Tool(dbToolB);


		}
		if (diffH == 3 )
		{
			dbHole = new DbHole( secondhit-.5,firstWhit+1 , 0., 1.e-1, 0. );
			dbHole.Tool(dbToolB);
			dbHole = new DbHole( secondhit-.5,firstWhit , 0., 1.e-1, 0. );
			dbHole.Tool(dbToolB);
			dbHole = new DbHole( secondhit-1.5,firstWhit+1 , 0., 1.e-1, 0. );
			dbHole.Tool(dbToolB);
			dbHole = new DbHole( secondhit-1.5,firstWhit , 0., 1.e-1, 0. );
			dbHole.Tool(dbToolB);

		}
		if(diffH == 10 )
		{
			dbHole = new DbHole( secondhit-2.5,SUfirstWhit-1 , 0., 1.e-1, 0. );
			dbHole.Tool(dbToolB);
			dbHole = new DbHole( secondhit-1,firstWhit , 0., 1.e-1, 0. );
			dbHole.IntSet( "PressureFoot", 1 );
			orient = 90;
			dbHole.DoubleSet("orient", orient);
			dbHole.Tool(dbToolA);
		}
		if (diffH == 2 )
		{

			dbHole = new DbHole( secondhit-.5,firstWhit+1 , 0., 1.e-1, 0. );
			dbHole.Tool(dbToolB);
			dbHole = new DbHole( secondhit-.5,firstWhit , 0., 1.e-1, 0. );
			dbHole.Tool(dbToolB);

		}
		if(diffH == 8 )
		{
			dbHole = new DbHole( secondhit-2.5,SUfirstWhit-1 , 0., 1.e-1, 0. );
			dbHole.Tool(dbToolB);
			dbHole = new DbHole( secondhit-1.5,SUfirstWhit-1 , 0., 1.e-1, 0. );
			dbHole.Tool(dbToolB);
			dbHole = new DbHole( secondhit-.5,SUfirstWhit-1 , 0., 1.e-1, 0. );
			dbHole.Tool(dbToolB);

			dbHole = new DbHole( secondhit-.5,firstWhit+3 , 0., 1.e-1, 0. );
			dbHole.Tool(dbToolB);
			dbHole = new DbHole( secondhit-.5,firstWhit+2 , 0., 1.e-1, 0. );
			dbHole.Tool(dbToolB);
			dbHole = new DbHole( secondhit-.5,firstWhit+1 , 0., 1.e-1, 0. );
			dbHole.Tool(dbToolB);
			dbHole = new DbHole( secondhit-.5,firstWhit , 0., 1.e-1, 0. );
			dbHole.Tool(dbToolB);
		}


	}

	private void CreateLogoLine()
	{
		DbLine		dbLine;
		DbTool	Logo_Line;
		Point ptA1;
		Point ptB1;
		Point ptA;
		Point ptB;
		Msg.Diagnostic( "DOING CREATE LOGO LINE");


		Msg.Diagnostic( "%%%%%%%%%%%%%%%%%----- nDoNotch---%%%%%%%%%%%%%%%%%" + nDoNotch);
		if(Holes_W_Notch())
		{

			if(DoLogo())
			{
				Logo_Line = new DbTool("Logo_Line");
				Logo_Line = Model.ToolGet("Logo_Line");
				Logo_Line.Color( DbEntity.RGB(113, 255, 51) );


				Msg.Diagnostic( "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%Into CreateLogoLine DoLogo true");

				ptA = new Point(ptK.X(),ptR.Y()-.375 - .190 - .1875,0);
				ptB1 = new Point(ptR.X(), ptR.Y()-.375 - .190 - .1875,0);

				ptB = new Point(ptR.X(), ptB1.Y(), 0. );

				LogoLine = new DbLine(ptA, ptB);

				Msg.Diagnostic( "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ptB X -- " + TempStartX + " and Y --- " + TempStartY);
				TempStartPt = LogoLine.StartPt();
				TempEndPt = LogoLine.EndPt();
				TempMidPt = LogoLine.MidPt();

				TempStartX = ptO.X();
				TempStartY = ptO.Y();
				TempPT = new Point(TempStartX,TempStartY, 0);
				Msg.Diagnostic( "#############################################################: ");
			}
			else
			{
				Logo_Line = new DbTool("Logo_Line");
				Logo_Line = Model.ToolGet("Logo_Line");
				Logo_Line.Color( DbEntity.RGB(113, 255, 51) );

				Msg.Diagnostic( "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%Into CreateLogoLine DoLogo False");

//Msg.Display("Set PtA");
				ptA = new Point(0, (profwid-.375 - .190 - .1875) ,0);

//Msg.Display("Set PtB");
				ptB = new Point(proflen, ptA.Y(), 0. );

//Msg.Display("Creat the line");
				LogoLine = new DbLine(ptA, ptB);
//Msg.Display("Logo Line done");

			}



		}
		else
		{
			Point ptBB;

			if(DoLogo())
			{
				Logo_Line = new DbTool("Logo_Line");
				Logo_Line = Model.ToolGet("Logo_Line");
				Logo_Line.Color( DbEntity.RGB(113, 255, 51) );


				ptA = new Point(0,  TopMidPt.Y() -.375 - .190 - .1875,0);
				ptB = new Point(proflen, TopMidPt.Y() -.375 - .190 - .1875,0);




				Msg.Diagnostic( "%%%%%%%%%%%%%%%%%----- DID WE GET THIS FAR---%%%%%%%%%%%%%%%%%");
				LogoLine = new DbLine( ptA, ptB);

				TempStartPt = LogoLine.StartPt();
				TempEndPt = LogoLine.EndPt();
				TempMidPt = LogoLine.MidPt();

				TempStartX = TempStartPt.X();
				TempStartY = TempStartPt.Y();
				TempPT = new Point(TempStartX,TempStartY, 0);
				Msg.Diagnostic( "#############################################################: ");
				Msg.Diagnostic("TempStartY() __ " + TempStartY);
			}
			else
			{

				ptA = new Point(0,  profwid,0);
				ptB = new Point(proflen, profwid,0);




				Msg.Diagnostic( "%%%%%%%%%%%%%%%%%----- DID WE GET THIS FAR---%%%%%%%%%%%%%%%%%");
				LogoLine = new DbLine( ptA, ptB);

				TempStartPt = LogoLine.StartPt();
				TempEndPt = LogoLine.EndPt();
				TempMidPt = LogoLine.MidPt();

				TempStartX = TempStartPt.X();
				TempStartY = TempStartPt.Y();
				TempPT = new Point(TempStartX,TempStartY, 0);
				Msg.Diagnostic( "#############################################################: ");
				Msg.Diagnostic("TempStartY() __ " + TempStartY);
			}
		}

	}


	private double YLeftOver()
	{
			//calculate how much material is left ove in the Y axis.
			//The YLeftove represents taking the width of the material
			// and subtracting the width of the panel. The we also
			//Subtract the width of the deadzone.
			//If the value is => 1 then we know we have at least 1 inch of material
			//left ove on the top of the sheet

			double TheValue;

			TheValue = ((matwidth - TopMidPt.Y()) - DeadZoneWid );


			return TheValue;


	}


	private boolean DoLogo()
	{
		int mDoLogo = Model.IntGet("mac.Logo");

		Msg.Diagnostic( "**************************: ");
		Msg.Diagnostic( "**************************: ");
		Msg.Diagnostic( "**************************: ");
		Msg.Diagnostic( "**************************: ");
		Msg.Diagnostic( "**************************: ");
		Msg.Diagnostic( "**************************: ");


       	Msg.Diagnostic( "mDoLogo -- Before Value: " + mDoLogo );

		Msg.Diagnostic( "**************************: ");
		Msg.Diagnostic( "**************************: ");
		Msg.Diagnostic( "**************************: ");
		Msg.Diagnostic( "**************************: ");
		Msg.Diagnostic( "**************************: ");
		Msg.Diagnostic( "**************************: ");
		Msg.Diagnostic( "**************************: ");


		if(mDoLogo == 1)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	private boolean Holes_W_Notch()
	{
		int nDoNotch = Model.IntGet("mac.Notch");

		if (nDoNotch == 1)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	private void DoCodeGen()
	{
		String	sMyFile = Model.StringGet("ncfile");
		cgfile = Model.StringGet("cgfile");
		String	cmd;

		//Msg.Display(sMyFile);


		Model.StringSet("ncfile",sMyFile);
		Model.StringSet("cg.comment1","");
		Model.StringSet("cg.units","Metric");
		Model.StringSet("cgfile", cgfile);
		Model.IntSet("OptAlgorithm",1);
		Model.IntSet("opttoolpath",1);
		Model.IntSet("Sticky",10);
		Model.IntSet("subs",0);

		// cg.xload|dbl|Load X|14.955
// cg.yload|dbl|Load Y|5.512

//		Portal.Execute("Admin:GlobalModel:");
//
//		Msg.Display("Admin:GlobalModel:");
//		//Portal.Execute("codegen:generate:cgfile=\"C:\\Program Files (x86)\\WE-CIM\\22.0\\CodeGen\\TRUMPF2020_D.java\",ncfile=\"Z:\\Custom_Progs\\1234.LST\",Subs=0,adv=0");
//		//cmd = "codegen:generate:cgfile=\"C:\\Program Files (x86)\\WE-CIM\\22.0\\CodeGen\\TRUMPF2020_D.java\",ncfile=\"c:\\Program Files\\WE-CIM\\22.0\\cnc\\1234.LST\",Subs=0,adv=0";
//		cmd="codegen:generate:cgfile=" + cgfile + ",ncfile=" + sMyFile + ",Subs=0,adv=0,Optalgorithm=1,Slice_Opt=0,Sticky=1,BiDir=1,Seq_Opt=2,AllHolesFirst=1,HolesByToolOrder=1,ByCompletePart=1,HolesAcrossLocalNest=0";
//
//		Portal.Execute(cmd);
//Msg.Display(cmd);
//		Portal.Execute("Admin:GlobalModel:");
//Msg.Display("Admin:GlobalModel:Again");

		//Portal.Execute("codegen:generate:cgfile=\"C:\\Program Files (x86)\\WE-CIM\\22.0\\CodeGen\\TRUMPF2020_D.java\",ncfile=\"c:\\Program Files\\WE-CIM\\22.0\\cnc\\01234.cnc\",Subs=0,adv=0,Optalgorithm=1,Slice_Opt=2,Sticky=10,BiDir=1,Seq_Opt=0");

	//	Msg.Display("CNC File Created");
	}


	private void DoClamps()
	{
		int zone_id;

		DbLine dbLine;

		//Clamp Info
		double clamp1pos = Model.DoubleGet("Clamp1Pos");
		double clamp2pos = Model.DoubleGet("Clamp2Pos");
		double clamplength = Model.DoubleGet("Clamp_Length");
		double clampwidth = Model.DoubleGet("Clamp_Width");

		TopLN=Model.EntityGet("TopLine");

		TopName = DbLine.DbLine( TopLN);

		TempStartPt = TopName.StartPt();

			Model.DoubleSet("mac.repoamt",(TempStartPt.X() + 20));
		if(matlength > 73)
		{
			clamp2 = 17;

		//	Model.DoubleSet("mac.repoamt", matlength - proflen);
		}
		else
		{
			Model.DoubleSet("mac.repoamt", -9.0);
		}

		RepoAmt =( Model.DoubleGet("mac.repoamt") );

		if (DEBUG) Msg.Display( "The repo amt is <" + RepoAmt + " >");

		//Msg.Display("Before Messng with repoamt is < " + RepoAmt + " >");

		Model.DoubleSet("mac.holdX", matlength/2);
		Model.DoubleSet("mac.holdY", matwidth/2);

		//double startclamp = -7.08711;
		double pos1 = -7.08711 + (5.118 * clamp1);
		double pos2 = -7.08711 + (5.118 * clamp2) ;


		Model.IntSet("clamp1",clamp1);
		Model.IntSet("clamp2",clamp2);
		Model.IntSet("mac.clmp1", clamp1);
		Model.IntSet("mac.clmp2", clamp2);

		//Msg.Display("THE repoamt is < " + RepoAmt + " >");

		Portal.Execute( "Zone:Get:_zone_num=1");
		zone_id = Portal.IntGet("id");
		Portal.Execute("zone:update: id =" + zone_id + ",hold_type=0,_zone_num=1,_hold_x=" + (matlength/2) + ",_hold_y=" + (matwidth/2) + ",_zone_repo=" + RepoAmt * -1 );

		Portal.Execute( "Feature:Modify: id=" + zone_id + ",selected=1");
		Portal.Execute( "Create:Clamp: num=2,x1=" + pos1 + ",y1=0,x2=" + pos2 +",y2=0");
	}

	private void CreateLogo()
	{
		DbHole		dbHole;
		DbLine		dbLine;
		Point ptA1;
		Point ptB1;
		Point ptA;
		//Point ptB;


		Msg.Diagnostic( "**************************: ");
		Msg.Diagnostic( "Did we get into createlogo ");
		Msg.Diagnostic( "**************************: ");


		String	sval = Model.StringGet("mac.Logo");


		int nLogo  = Model.IntGet("mac.Logo");


			if(nDoNotch == 1)
			{

				if(DoLogo())
				{

					Msg.Diagnostic( "CreateLogo Notch True Logo True");

					dbHole = new DbHole(TopMidPt.X(),ptJ.Y()-.375, 0., 1.e-1, 0. );
					dbHole.Name("Logo");
					Logo = dbHole.Id();
					dbHole.Color(LogoTool.Color());
					dbHole.Tool( LogoTool );
					dbHole.DoubleSet( "orient", 0.0  );

				}
				else
				{

				}
			}
			else
			{
				Msg.Diagnostic( "Into Else statement");

				Msg.Diagnostic( "TopMidPt location is " + TopMidPt.X() + " and " + TopMidPt.Y());

				dbHole = new DbHole(TopMidPt.X(),TopMidPt.Y() - .190 - .1875, 0., 1.e-1, 0. );
				Msg.Diagnostic( "Created the hole");

				dbHole.Color(LogoTool.Color());

				Msg.Diagnostic( "setting the color");
				dbHole.Tool( LogoTool );

				Msg.Diagnostic( "Setting the tool");

				dbHole.DoubleSet( "orient", 0.0  );

				Msg.Diagnostic( "setting orient");

				 ptA1 = new Point (dbHole.CenterPt());
				 Msg.Diagnostic( "Setting ptA1 to a value of " + ptA1.Y());

				 ptA = new Point (0,ptA1.Y() - .190 - .1875,0);

				 ptB = new Point(proflen, ptA1.Y() - .190 - .1875,0);
				 Msg.Diagnostic( "Setting ptB1 to a valoe of "+ ptB.Y());


				//LogoLine = new DbLine(ptA, ptB);

			}
	}

	private void CreateLL_UL_NotchHits()
	{
		DbEntity	Hole1st45;
		DbEntity	Hole3rd45;;
		Model.ActiveToolSet( Sqr1000 );

		Point ptHole;

		DbHole		dbHole;

		ptHole = new Point( ptB.X() - .5, ptB.Y()-.5 , 0. );
		dbHole = new DbHole(  ptHole.X(),  ptHole.Y(), 0, 1.e-4, 0 );
		dbHole.Name("Sqr1000_Hole1");
		FirstSqY = ptHole.Y();
		dbHole.Tool( Sqr1000);
		dbHole.DoubleSet( "orient", 0.0  );
		dbHole.Color(Sqr1000.Color());
		Sqr1000_Hole1 = dbHole.Id();

		ptHole = new Point( ptHole.X()-.9, ptB.Y()-.5 , 0. );
		dbHole = new DbHole(  ptHole.X(),  ptHole.Y(), 0, 1.e-4, 0 );
		dbHole.Name("Sqr1000_Hole2");
		dbHole.Tool( Sqr1000);
		dbHole.DoubleSet( "orient", 0.0  );
		dbHole.Color(Sqr1000.Color());
		Sqr1000_Hole2 = dbHole.Id();

		ptHole = new Point(ptF.X() - .5, ptF.Y() - 1.4 , 0. );
		dbHole = new DbHole(  ptHole.X(),  ptHole.Y(), 0, 1.e-4, 0 );
		dbHole.Name("Sqr1000_Hole3");
		dbHole.Tool( Sqr1000);
		dbHole.DoubleSet( "orient", 0.0  );
		dbHole.Color(Sqr1000.Color());
		Sqr1000_Hole3 = dbHole.Id();

		ptHole = new Point( ptF.X() - .5, ptF.Y() - .5 , 0. );
		dbHole = new DbHole(  ptHole.X(),  ptHole.Y(), 0, 1.e-4, 0 );
		dbHole.Name("Sqr1000_Hole4");
		dbHole.Tool( Sqr1000);
		dbHole.DoubleSet( "orient", 0.0  );
		dbHole.Color(Sqr1000.Color());
		Sqr1000_Hole4 = dbHole.Id();

		ptHole = new Point( ptD.X()-.5, ptD.Y()-.5 , 0. );
		dbHole = new DbHole(  ptHole.X(),  ptHole.Y(), 0, 1.e-4, 0 );
		dbHole.Name("Sqr1000_Hole5");
		dbHole.Tool( Sqr1000);
		dbHole.Color(Sqr1000.Color());
		Sqr1000_Hole5 = dbHole.Id();

		ptHole = new Point( ptF.X()-.7072, ptF.Y() , 0. );
		dbHole = new DbHole(  ptHole.X(),  ptHole.Y(), 0, 1.e-4, 0 );
		dbHole.Name("Sqr1000_Hole6");
		dbHole.Tool( Sqr1000);
		dbHole.DoubleSet( "orient", 45  );
		dbHole.Color(Sqr1000.Color());
		Sqr1000_Hole6 = dbHole.Id();

		ptHole = new Point( ptI.X()-.7072, ptI.Y() , 0. );
		dbHole = new DbHole(  ptHole.X(),  ptHole.Y(), 0, 1.e-4, 0 );
		dbHole.Name("Sqr1000_Hole7");
		dbHole.Tool( Sqr1000);
		dbHole.DoubleSet( "orient", 45  );
		dbHole.Color(Sqr1000.Color());
		Sqr1000_Hole7 = dbHole.Id();

		ptHole = new Point( ptI.X()-.5, ptI.Y() + .5 , 0. );
		dbHole = new DbHole(  ptHole.X(),  ptHole.Y(), 0, 1.e-4, 0 );
		dbHole.Name("Sqr1000_Hole8");
		dbHole.Tool( Sqr1000);
		dbHole.DoubleSet( "orient", 0.0  );
		dbHole.Color(Sqr1000.Color());
		Sqr1000_Hole8 = dbHole.Id();

		ptHole = new Point( ptI.X() - .5, ptI.Y() + 1.4 , 0. );
		dbHole = new DbHole(  ptHole.X(),  ptHole.Y(), 0, 1.e-4, 0 );
		dbHole.Name("Sqr1000_Hole9");
		dbHole.Tool( Sqr1000);
		dbHole.DoubleSet( "orient", 0.0  );
		dbHole.Color(Sqr1000.Color());
		Sqr1000_Hole9 = dbHole.Id();

		ptHole = new Point( ptK.X() - .5, ptK.Y() + .5 , 0. );
		dbHole = new DbHole(  ptHole.X(),  ptHole.Y(), 0, 1.e-4, 0 );
		dbHole.Name("Sqr1000_Hole10");
		dbHole.Tool( Sqr1000);
		dbHole.DoubleSet( "orient", 0.0  );
		dbHole.Color(Sqr1000.Color());
		Sqr1000_Hole10 = dbHole.Id();

		ptHole = new Point( ptM.X() - .5, ptM.Y() + .5 , 0. );
		dbHole = new DbHole(  ptHole.X(),  ptHole.Y(), 0, 1.e-4, 0 );
		dbHole.Name("Sqr1000_Hole11");
		dbHole.Tool( Sqr1000);
		dbHole.DoubleSet( "orient", 0.0  );
		dbHole.Color(Sqr1000.Color());
		Sqr1000_Hole11 = dbHole.Id();

		ptHole = new Point( ptM.X() - 1.4, ptM.Y() + .5 , 0. );
		dbHole = new DbHole(  ptHole.X(),  ptHole.Y(), 0, 1.e-4, 0 );
		dbHole.Name("Sqr1000_Hole12");
		dbHole.Tool( Sqr1000);
		dbHole.DoubleSet( "orient", 0.0  );
		dbHole.Color(Sqr1000.Color());
		Sqr1000_Hole12 = dbHole.Id();

		ptHole = new Point( ptP.X() + .5, ptP.Y() + .5 , 0. );
		dbHole = new DbHole(  ptHole.X(),  ptHole.Y(), 0, 1.e-4, 0 );
		dbHole.Name("Sqr1000_Hole13");
		dbHole.Tool( Sqr1000);
		dbHole.DoubleSet( "orient", 0.0  );
		dbHole.Color(Sqr1000.Color());
		Sqr1000_Hole13 = dbHole.Id();

		ptHole = new Point( ptP.X() + 1.4, ptP.Y() + .5 , 0. );
		dbHole = new DbHole(  ptHole.X(),  ptHole.Y(), 0, 1.e-4, 0 );
		dbHole.Name("Sqr1000_Hole14");
		dbHole.Tool( Sqr1000);
		dbHole.DoubleSet( "orient", 0.0  );
		dbHole.Color(Sqr1000.Color());
		Sqr1000_Hole14 = dbHole.Id();

		ptHole = new Point( ptR.X() + .5, ptR.Y() + .5 , 0. );
		dbHole = new DbHole(  ptHole.X(),  ptHole.Y(), 0, 1.e-4, 0 );
		dbHole.Name("Sqr1000_Hole15");
		dbHole.Tool( Sqr1000);
		dbHole.DoubleSet( "orient", 0.0  );
		dbHole.Color(Sqr1000.Color());
		Sqr1000_Hole15 = dbHole.Id();

		ptHole = new Point( ptT.X() + .5, ptT.Y() + 1.4 , 0. );
		dbHole = new DbHole(  ptHole.X(),  ptHole.Y(), 0, 1.e-4, 0 );
		dbHole.Name("Sqr1000_Hole17");
		dbHole.Tool( Sqr1000);
		dbHole.DoubleSet( "orient", 0.0  );
		dbHole.Color(Sqr1000.Color());
		Sqr1000_Hole17 = dbHole.Id();

		ptHole = new Point( ptT.X() + .5, ptT.Y() + .5 , 0. );
		dbHole = new DbHole(  ptHole.X(),  ptHole.Y(), 0, 1.e-4, 0 );
		dbHole.Name("Sqr1000_Hole18");
		dbHole.Tool( Sqr1000);
		dbHole.DoubleSet( "orient", 0.0  );
		dbHole.Color(Sqr1000.Color());
		Sqr1000_Hole18 = dbHole.Id();

		ptHole = new Point( ptT.X() + .7072, ptT.Y()  , 0. );
		dbHole = new DbHole(  ptHole.X(),  ptHole.Y(), 0, 1.e-4, 0 );
		dbHole.Name("Sqr1000_Hole19");
		dbHole.Tool( Sqr1000);
		dbHole.DoubleSet( "orient",45);
		dbHole.Color(Sqr1000.Color());
		Sqr1000_Hole19 = dbHole.Id();


		ptHole = new Point( ptW.X() + .7072, ptW.Y()  , 0. );
		dbHole = new DbHole(  ptHole.X(),  ptHole.Y(), 0, 1.e-4, 0 );
		dbHole.Name("Sqr1000_Hole20");
		dbHole.Tool( Sqr1000);
		dbHole.DoubleSet( "orient",45);
		dbHole.Color(Sqr1000.Color());
		Sqr1000_Hole20 = dbHole.Id();

		ptHole = new Point( ptW.X() + .5, ptW.Y() - .5 , 0. );
		dbHole = new DbHole(  ptHole.X(),  ptHole.Y(), 0, 1.e-4, 0 );
		dbHole.Name("Sqr1000_Hole21");
		dbHole.Tool( Sqr1000);
		dbHole.DoubleSet( "orient",0.0);
		dbHole.Color(Sqr1000.Color());
		Sqr1000_Hole21 = dbHole.Id();

		ptHole = new Point( ptY.X() + .5, ptY.Y() - .5 , 0. );
		dbHole = new DbHole(  ptHole.X(),  ptHole.Y(), 0, 1.e-4, 0 );
		dbHole.Name("Sqr1000_Hole22");
		dbHole.Tool( Sqr1000);
		dbHole.DoubleSet( "orient",0.0);
		dbHole.Color(Sqr1000.Color());
		Sqr1000_Hole22 = dbHole.Id();

		ptHole = new Point( ptW.X() + .5, ptW.Y() - 1.4 , 0. );
		dbHole = new DbHole(  ptHole.X(),  ptHole.Y(), 0, 1.e-4, 0 );
		dbHole.Name("Sqr1000_Hole23");
		dbHole.Tool( Sqr1000);
		dbHole.DoubleSet( "orient",0.0);
		dbHole.Color(Sqr1000.Color());
		Sqr1000_Hole23 = dbHole.Id();

		ptHole = new Point( ptAA.X() + 1.4, ptAA.Y() - .5 , 0. );
		dbHole = new DbHole(  ptHole.X(),  ptHole.Y(), 0, 1.e-4, 0 );
		dbHole.Name("Sqr1000_Hole24");
		dbHole.Tool( Sqr1000);
		dbHole.DoubleSet( "orient",0.0);
		dbHole.Color(Sqr1000.Color());
		Sqr1000_Hole24 = dbHole.Id();

		ptHole = new Point( ptAA.X() + .5, ptAA.Y() - .5 , 0. );
		dbHole = new DbHole(  ptHole.X(),  ptHole.Y(), 0, 1.e-4, 0 );
		dbHole.Name("Sqr1000_Hole25");
		dbHole.Tool( Sqr1000);
		dbHole.DoubleSet( "orient",0.0);
		dbHole.Color(Sqr1000.Color());
		Sqr1000_Hole25 = dbHole.Id();

		Selector.Flush();
		Selector.All( false );
		Selector.Tool( true);
		Selector.Hole( true);
		Selector.AddAllRefsTo( Sqr1000 );

		Selector.AddAllRefsTo( Sqr1000 );

		if(Holes_W_Notch() )
		{
			Msg.Diagnostic( "<<<<<<<<<<<<<<<WE ARE CREATUING NOTCHES>>>>>>>>>>>>>>>>>>>>>>>>>");

			if(nPanelQty > 1)
			{

				Msg.Diagnostic( "<<<<<<<<<<<<<<<THE QTY IS MORE THAN ONE>>>>>>>>>>>>>>>>>>>>>>>>>");

				//We save the entities here because it is just easier to get them from the Selection;
				//
				Notch_1_45 = Selector.Get(1);
				Notch_3_45 = Selector.Get(9);

			}

		}

		Selector.Flush();


	}

	private void Create45ToolPath()
	{
		int Sqr373_ID = Sqr373.Id();
		Portal.Execute("*selector:select:id=" + foofirst45);
		Portal.Execute("*selector:select:id=" + foo4th45);
		Portal.Execute("*selector:select:id=" + foo5th45);


		Portal.Execute("*selector:select:id=" + foo8th45);

		Portal.Execute("toolpath:autoindexoffset: toolid=" + Sqr373_ID + ",profid=" + foofirst45 + ",dir =1,longside = 1");
		int first45TP = Portal.IntGet("id");
		Portal.Execute("*Feature:Entity:id=" + first45TP + ",index=0");
		first45TP=Portal.IntGet("id");
		Portal.Execute("Entity:Name: id=" + first45TP + ",name= First45ToolPath");


		Portal.Execute("toolpath:autoindexoffset: toolid=" + Sqr373_ID + ",profid=" + foo4th45 + ",dir =1,longside = 1");
		int second45TP = Portal.IntGet("id");
		Portal.Execute("*Feature:Entity:id=" + second45TP + ",index=0");
		second45TP=Portal.IntGet("id");
		Portal.Execute("Entity:Name: id=" + second45TP + ",name= Secodnd45ToolPath");

		Portal.Execute("toolpath:autoindexoffset: toolid=" + Sqr373_ID + ",profid=" + foo5th45 + ",dir =1,longside = 1");
		int third45TP = Portal.IntGet("id");
		Portal.Execute("*Feature:Entity:id=" + third45TP + ",index=0");
		third45TP=Portal.IntGet("id");
		Portal.Execute("Entity:Name: id=" + third45TP + ",name= Third45ToolPath");

		Portal.Execute("toolpath:autoindexoffset: toolid=" + Sqr373_ID + ",profid=" + foo8th45 + ",dir =1,longside = 1");
		int fifth45TP = Portal.IntGet("id");
		Portal.Execute("*Feature:Entity:id=" + fifth45TP + ",index=0");
		fifth45TP=Portal.IntGet("id");
		Portal.Execute("Entity:Name: id=" + fifth45TP + ",name= Fifth45ToolPath");


	}


	private void CreateOutsidNoNotch()
	{

			DbLine dbline;
			DbEntity	dbEntity;
			Point origin = new Point( 0, 0, 0);

			Msg.Diagnostic( "**************************: ");
			if (DEBUG) Msg.Display( " CreateOutsidNoNotch " );
			Msg.Diagnostic( "**************************: ");

			magpanel = new DbTool("Part_Outline");
			magpanel = Model.ToolGet("Part_Outline");
			magpanel.Color( DbEntity.RGB(113, 255, 51) );

			if (DEBUG) Msg.Display( "CreateOutsidNoNotch did we get here" );
			DbProfile rect = Shape.Rectangle( origin, proflen, profwid, 0, Const.CW );




	}
	private void CreateOutsideProfile()
	{

			DbLine dbline;
			DbEntity	dbEntity;
			int profID;
			DbProfile rect = new DbProfile();


			Msg.Diagnostic( "**************************: ");
			Msg.Diagnostic( "CreateOutsideProfile " );
			Msg.Diagnostic( "**************************: ");


			if(nDoNotch == 1 )
			{

				Msg.Diagnostic( "**************************: ");
				Msg.Diagnostic( "checking do notch i create putside profile " );
				Msg.Diagnostic( "**************************: ");

				magpanel = new DbTool("Part_Outline");
				magpanel = Model.ToolGet("Part_Outline");
				magpanel.Color( DbEntity.RGB(113, 255, 51) );

				xc = xref;
				yc = yref;

				Point ptA = new Point((len45 + len45 + notchwidth + notchwidth) ,0, 0. );
				ShakerTab1= ptA;
				//Portal.Execute( "create:command x=" + ptA.X() +",y=" +ptA.Y() +",z=0,pos=2,angle=0,cmd=" + DOUBLE_QUOTE + " Point A" + DOUBLE_QUOTE);

				ptB = new Point(ptA.X() - len45, ptA.Y() + len45, 0. );
				//Portal.Execute( "create:command x=" + ptB.X() +",y=" +ptB.Y() +",z=0,pos=2,angle=0,cmd=" + DOUBLE_QUOTE + " Point B" + DOUBLE_QUOTE);

				ptC = new Point(ptB.X() - notchwidth , ptB.Y(), 0. );
				//Portal.Execute( "create:command x=" + ptC.X() +",y=" +ptC.Y() +",z=0,pos=2,angle=0,cmd=" + DOUBLE_QUOTE + " Point C" + DOUBLE_QUOTE);

				ptD = new Point( ptC.X(), ptC.Y() + notchwidth, 0. );
				//Portal.Execute( "create:command x=" + ptD.X() +",y=" +ptD.Y() +",z=0,pos=2,angle=0,cmd=" + DOUBLE_QUOTE + " Point D" + DOUBLE_QUOTE);

				ptE = new Point(ptD.X() - notchwidth, ptD.Y(), 0. );
				//Portal.Execute( "create:command x=" + ptE.X() +",y=" +ptE.Y() +",z=0,pos=2,angle=0,cmd=" + DOUBLE_QUOTE + " Point E" + DOUBLE_QUOTE);

				ptF = new Point( ptE.X() , ptE.Y() + notchwidth, 0. );
				//Portal.Execute( "create:command x=" + ptF.X() +",y=" +ptF.Y() +",z=0,pos=2,angle=0,cmd=" + DOUBLE_QUOTE + " Point F" + DOUBLE_QUOTE);

				ptG = new Point( 0, ptF.Y() + len45, 0. );
				//Portal.Execute( "create:command x=" + ptG.X() +",y=" +ptG.Y() +",z=0,pos=2,angle=0,cmd=" + DOUBLE_QUOTE + " Point G" + DOUBLE_QUOTE);
				ShakerTab2 = ptG;

				ptH = new Point( ptG.X(), ptG.Y() + ( profwid - twicelen45 - twicenotchwidth), 0. );
				//Portal.Execute( "create:command x=" + ptH.X() +",y=" +ptH.Y() +",z=0,pos=2,angle=0,cmd=" + DOUBLE_QUOTE + " Point H" + DOUBLE_QUOTE);
				ShakerTab3 = ptH;

				ptI = new Point( len45, ptH.Y() + len45, 0. );
				//Portal.Execute( "create:command x=" + ptI.X() +",y=" +ptI.Y() +",z=0,pos=2,angle=0,cmd=" + DOUBLE_QUOTE + " Point I" + DOUBLE_QUOTE);

				ptJ = new Point( ptI.X(), ptI.Y() + notchwidth, 0. );
				//Portal.Execute( "create:command x=" + ptJ.X() +",y=" +ptJ.Y() +",z=0,pos=2,angle=0,cmd=" + DOUBLE_QUOTE + " Point J" + DOUBLE_QUOTE);

				ptK = new Point( ptJ.X() + notchwidth, ptJ.Y(), 0. );
				//Portal.Execute( "create:command x=" + ptK.X() +",y=" +ptK.Y() +",z=0,pos=2,angle=0,cmd=" + DOUBLE_QUOTE + " Point K" + DOUBLE_QUOTE);

				ptL = new Point( ptK.X(), ptK.Y() + notchwidth, 0. );
				//Portal.Execute( "create:command x=" + ptL.X() +",y=" +ptL.Y() +",z=0,pos=2,angle=0,cmd=" + DOUBLE_QUOTE + " Point L" + DOUBLE_QUOTE);

				ptM = new Point( ptL.X() + notchwidth, ptL.Y() , 0. );
				//Portal.Execute( "create:command x=" + ptM.X() +",y=" +ptM.Y() +",z=0,pos=2,angle=0,cmd=" + DOUBLE_QUOTE + " Point M" + DOUBLE_QUOTE);

				ptN = new Point( ptM.X() + len45, ptM.Y() + len45, 0. );
				//Portal.Execute( "create:command x=" + ptN.X() +",y=" +ptN.Y() +",z=0,pos=2,angle=0,cmd=" + DOUBLE_QUOTE + " MY POINT N" + DOUBLE_QUOTE);
				ShakerTab4 = ptN;

				ptO = new Point( ptN.X() +(proflen - twicenotchwidth-twicelen45 ), ptN.Y() , 0. );
				//Portal.Execute( "create:command x=" + ptO.X() +",y=" +ptO.Y() +",z=0,pos=2,angle=0,cmd=" + DOUBLE_QUOTE + " MY POINT O" + DOUBLE_QUOTE);
				ShakerTab5 = ptO;

				ptP = new Point( ptO.X() + len45, ptO.Y() - len45 , 0. );
				//Portal.Execute( "create:command x=" + ptP.X() +",y=" +ptP.Y() +",z=0,pos=2,angle=0,cmd=" + DOUBLE_QUOTE + " MY POINT P" + DOUBLE_QUOTE);

				ptQ = new Point( ptP.X() + notchwidth, ptP.Y() , 0. );
				//Portal.Execute( "create:command x=" + ptQ.X() +",y=" +ptQ.Y() +",z=0,pos=2,angle=0,cmd=" + DOUBLE_QUOTE + " MY POINT Q" + DOUBLE_QUOTE);

				ptR = new Point( ptQ.X(), ptQ.Y() - notchwidth , 0. );
				//Portal.Execute( "create:command x=" + ptR.X() +",y=" +ptR.Y() +",z=0,pos=2,angle=0,cmd=" + DOUBLE_QUOTE + " MY POINT R" + DOUBLE_QUOTE);

				ptS = new Point( ptR.X() + notchwidth, ptR.Y() , 0. );
				//Portal.Execute( "create:command x=" + ptS.X() +",y=" +ptS.Y() +",z=0,pos=2,angle=0,cmd=" + DOUBLE_QUOTE + " MY POINT S" + DOUBLE_QUOTE);

				ptT = new Point( ptS.X() , ptS.Y() - notchwidth , 0. );
				//Portal.Execute( "create:command x=" + ptT.X() +",y=" +ptT.Y() +",z=0,pos=2,angle=0,cmd=" + DOUBLE_QUOTE + " MY POINT T" + DOUBLE_QUOTE);

				ptU = new Point( ptT.X()  + len45, ptT.Y() - len45 , 0. );
				//Portal.Execute( "create:command x=" + ptU.X() +",y=" +ptU.Y() +",z=0,pos=2,angle=0,cmd=" + DOUBLE_QUOTE + " MY POINT U" + DOUBLE_QUOTE);
				ShakerTab6 = ptU;

				ptV = new Point( ptU.X() , ptU.Y() - ( profwid - twicelen45 - twicenotchwidth) , 0. );
				//Portal.Execute( "create:command x=" + ptV.X() +",y=" +ptV.Y() +",z=0,pos=2,angle=0,cmd=" + DOUBLE_QUOTE + " MY POINT V" + DOUBLE_QUOTE);
				ShakerTab7 = ptV;

				ptW = new Point( ptV.X() - len45, ptV.Y() - len45 , 0. );
				//Portal.Execute( "create:command x=" + ptW.X() +",y=" +ptW.Y() +",z=0,pos=2,angle=0,cmd=" + DOUBLE_QUOTE + " MY POINT W" + DOUBLE_QUOTE);

				ptX = new Point( ptW.X() , ptW.Y() - notchwidth , 0. );
				//Portal.Execute( "create:command x=" + ptX.X() +",y=" +ptX.Y() +",z=0,pos=2,angle=0,cmd=" + DOUBLE_QUOTE + " MY POINT X" + DOUBLE_QUOTE);

				ptY = new Point( ptX.X() - notchwidth, ptX.Y() , 0. );
				//Portal.Execute( "create:command x=" + ptY.X() +",y=" +ptY.Y() +",z=0,pos=2,angle=0,cmd=" + DOUBLE_QUOTE + " MY POINT Y" + DOUBLE_QUOTE);

				ptZ = new Point( ptY.X(), ptY.Y() - notchwidth , 0. );
				//Portal.Execute( "create:command x=" + ptZ.X() +",y=" +ptZ.Y() +",z=0,pos=2,angle=0,cmd=" + DOUBLE_QUOTE + " MY POINT Z" + DOUBLE_QUOTE);

				ptAA = new Point( ptZ.X() -notchwidth, ptZ.Y() , 0. );
				//Portal.Execute( "create:command x=" + ptAA.X() +",y=" +ptAA.Y() +",z=0,pos=2,angle=0,cmd=" + DOUBLE_QUOTE + " MY POINT AA" + DOUBLE_QUOTE);

				ptAB = new Point( ptAA.X() -len45, ptAA.Y() -len45 , 0. );
				//Portal.Execute( "create:command x=" + ptAB.X() +",y=" +ptAB.Y() +",z=0,pos=2,angle=0,cmd=" + DOUBLE_QUOTE + " MY POINT AB" + DOUBLE_QUOTE);
				ShakerTab8 = ptAB;

				profID = Portal.IntGet("id");

				dbline = new DbLine( ptA, ptB );
				dbline.Name("First45");
				foofirst45 = Portal.IntGet("id");
				if (DEBUG) Msg.Display( "foofirst45 = " + foofirst45);
				rect.Append(dbline);

				rect.Append( new DbLine( ptB, ptC ) );
				rect.Append( new DbLine( ptC, ptD ) );
				rect.Append( new DbLine( ptD, ptE ) );
				rect.Append( new DbLine( ptE, ptF ) );

				dbline = new DbLine(ptF, ptG);
				foo2nd45 = Portal.IntGet("id");
				if (DEBUG) Msg.Display( "foo2nd45 = " + foo2nd45);
				rect.Append(dbline);

				dbline = new DbLine( ptG, ptH );
				LeftMidPt=dbline.MidPt();
				dbline.Name("LeftLine");
				LeftID = Portal.IntGet("id");

				rect.Append(dbline);

				dbline = new DbLine( ptH, ptI );
				foo3rd45 = Portal.IntGet("id");
				if (DEBUG) Msg.Display( "foo3rd45 = " + foo3rd45);
				rect.Append(dbline);

				rect.Append( new DbLine( ptI, ptJ ) );

				rect.Append( new DbLine( ptJ, ptK ) );
				rect.Append( new DbLine( ptK, ptL ) );
				rect.Append( new DbLine( ptL, ptM ) );

				dbline = new DbLine( ptM, ptN );

				foo4th45 = Portal.IntGet("id");
				if (DEBUG) Msg.Display( "foo4th45 = " + foo4th45);
				rect.Append(dbline);

				dbline = new DbLine( ptN, ptO );
				dbline.Name("TopLine");
				TopMidPt = dbline.MidPt();
				TopID = Portal.IntGet("id");

				dMoveFromY = ptO.Y();
				rect.Append(dbline);

				dbline = new DbLine( ptO, ptP );


				foo5th45 = Portal.IntGet("id");
				if (DEBUG) Msg.Display( "foo5th45 = " + foo5th45);
				rect.Append(dbline);

				rect.Append( new DbLine( ptO, ptP ) );

				Msg.Diagnostic( "ptO start Y value -- " + ptO.Y() );

				rect.Append( new DbLine( ptP, ptQ ) );
				rect.Append( new DbLine( ptQ, ptR ) );
				rect.Append( new DbLine( ptR, ptS ) );
				rect.Append( new DbLine( ptS, ptT ) );


				dbline = new DbLine( ptT, ptU );
				foo6th45 = Portal.IntGet("id");
				if (DEBUG) Msg.Display( "foo6th45 = " + foo6th45);
				rect.Append(dbline);

				dMoveFromX = ptV.X();

				dbline = new DbLine( ptU, ptV );
				dbline.Name("RightLine");
				RightMidPt = dbline.MidPt();
				RightID = Portal.IntGet("id");

				RtLine = dbline;

				rect.Append( dbline );

				dbline = new DbLine( ptV, ptW );

				foo7th45 = Portal.IntGet("id");
				//if (DEBUG) Msg.Display( "foo7th45 = " + foo7th45);
				rect.Append(dbline);

				rect.Append( new DbLine( ptW, ptX ) );
				rect.Append( new DbLine( ptX, ptY ) );
				rect.Append( new DbLine( ptY, ptZ ) );
				rect.Append( new DbLine( ptZ, ptAA ));

				dbline = new DbLine( ptAA, ptAB );
				foo8th45 = Portal.IntGet("id");

				if (DEBUG) Msg.Display( "foo8th45 = " + foo8th45);
				rect.Append(dbline);
				dbline = new DbLine( ptAB, ptA );
				BottomID = Portal.IntGet("id");
				dbline.Name("BottomLine");
				BottomMidPt = dbline.MidPt();


				rect.Append( dbline );

				DbCurve		dbCurve;
				dbEntity = null;
				dbEntity = rect.Get(6);
				dbCurve = DbCurve.DbCurve( dbEntity );
				LeftMidPt = dbCurve.MidPt();

			}

			else
			{

				magpanel = new DbTool("Part_Outline");
				magpanel = Model.ToolGet("Part_Outline");
				magpanel.Color( DbEntity.RGB(113, 255, 51) );
				double xc, yc;

				xc =  proflen;
				yc =  profwid;


				DbProfile dbProfile = new DbProfile();
				profID = Portal.IntGet("id");

				ptA = new Point(0,0, 0. );
				ptB = new Point(0,  profwid, 0. );

				ptC = new Point( proflen, profwid, 0. );
				ptD = new Point( proflen,0, 0. );

				dbline = new DbLine( ptA, ptB );
				LeftID = Portal.IntGet("id");
				LeftMidPt = dbline.MidPt();
				dbProfile.Append(dbline);



				dbline = new DbLine( ptB, ptC );
				TopID  = Portal.IntGet("id");
				dbProfile.Append(dbline);
				HorzLine = dbline;


				TopMidPt = dbline.MidPt();

				dbline = new DbLine( ptC, ptD );
				RightMidPt = dbline.MidPt();
				RightID = Portal.IntGet("id");
				RtLine = dbline;
				dbProfile.Append(dbline);


				dbline = new DbLine( ptD, ptA );
				BottomID = Portal.IntGet("id");
				dbProfile.Append(dbline);


			}

			Portal.Execute("*selector:select:id=" +profID + "\"");

			Portal.Execute("profile:explode:id=" + profID + ",Group=1");
            Portal.Execute("admin:regen:");
	}

	private void CreateBottomBendLines()
	{

		double reqNumOfLins = (proflen / .750);
		Double perValue = reqNumOfLins;
		int roundVal= (int) Math.round(perValue);
		int smallfeatureid, bigfeatureid;
		int entid, cnt, m_indx;
 		int featurecount;

		DbEntity dbEntity= null;

		DbFeature bl_bottom_Small = new DbFeature();
		smallfeatureid = Portal.IntGet("id");
		bl_bottom_Small.Name("bl_bottom_Small");



		DbTool	Bendlines;
		Bendlines = new DbTool("Bendlines");
		Bendlines = Model.ToolGet("Bendlines");
		Bendlines.Color( DbEntity.RGB(233, 51, 255) );

		//bottom side chamfer
		Point bendlineA = new Point(twicelen45 + twicelen45 , len45 , 0. );
		Point bendlineB = new Point(bendlineA.X() + (proflen - twicenotchwidth)  , bendlineA.Y(), 0. );

		DbLine LineA = new DbLine( bendlineA, bendlineB);
		entid = Portal.IntGet("id");

		bl_bottom_Small.Append(LineA);
		splitval = profwid;


		featurecount = bl_bottom_Small.Count();

		DbFeature bl_bottom_Big = new DbFeature();
		bigfeatureid = Portal.IntGet("id");
		bl_bottom_Big.Name("bl_bottom_Big");

		Bendlines = Model.ToolGet("Bendlines");
		Bendlines.Color( DbEntity.RGB(233, 51, 255) );

		//left side chamfer
		Point bendlineC = new Point(ptD.X(), ptD.Y() , 0. );
		Point bendlineD = new Point( ptD.X() + proflen, ptD.Y(), 0. );
		DbLine LineB  = new DbLine( bendlineC, bendlineD);
		entid = Portal.IntGet("id");

		bl_bottom_Big.Append(LineB);
		splitval = profwid;

		reqNumOfLins = ((profwid) / .750);

		perValue = reqNumOfLins;

		roundVal= (int) Math.round(reqNumOfLins);

		featurecount = bl_bottom_Big.Count();

	}

	private void CreateTopBendLines()
	{
		Msg.Diagnostic( "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%Into CreateTopBendLines");

		double reqNumOfLins = (proflen / .750);
		Double perValue = reqNumOfLins;
		int roundVal= (int) Math.round(perValue);
		int smallfeatureid, bigfeatureid;
		int entid, cnt, m_indx;
 		int featurecount;

		DbEntity dbEntity= null;

		DbFeature bl_top_Small = new DbFeature();
		smallfeatureid = Portal.IntGet("id");
		bl_top_Small.Name("bl_top_Small");


		DbTool	Bendlines;
		Bendlines = new DbTool("Bendlines");
		Bendlines = Model.ToolGet("Bendlines");
		Bendlines.Color( DbEntity.RGB(233, 51, 255) );

		//bottom side chamfer
		Point bendlineA = new Point(ptK.X() , ptK.Y(), 0. );
		Point bendlineB = new Point(ptK.X() + (proflen)  , ptK.Y(), 0. );


		DbLine LineA = new DbLine( bendlineA, bendlineB);
		bl_top_Small.Append(LineA);

		TopID = LineA.Id();
		TempPT = LineA.MidPt();

		entid = Portal.IntGet("id");


		if(nDoNotch == 1)
			{
				if (DoLogo() )
				{
					Msg.Diagnostic( "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@");
					Msg.Diagnostic( "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@");
					Msg.Diagnostic( "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@");

					Msg.Diagnostic( "Into Else Do Logo True Check inside of CreateTopBendLines");

					TempStartX = bendlineB.X();
					TempStartY = ptJ.Y()-.375;

				}
				else
				{
					Msg.Diagnostic( "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@");
					Msg.Diagnostic( "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@");
					Msg.Diagnostic( "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@");
					Msg.Diagnostic( "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@");

					Msg.Diagnostic( "Into Else Do Logo Flase Check inside of CreateTopBendLines");
					TopMidPt = LineA.MidPt();
					TempMidPt=LineA.MidPt();
					TempEndPt = LineA.EndPt();

				}


			}

		//do the 90 degree
		DbFeature bl_Top_Big = new DbFeature();
		bigfeatureid = Portal.IntGet("id");
		bl_Top_Big.Name("bl_Top_Big");

		Bendlines = Model.ToolGet("Bendlines");
		Bendlines.Color( DbEntity.RGB(233, 51, 255) );

		//left side chamfer
		Point bendlineC = new Point(ptM.X(), ptM.Y() , 0. );
		Point bendlineD = new Point( ptP.X(), ptP.Y(), 0. );

		DbLine LineB  = new DbLine( bendlineC, bendlineD);

	}

	private void CreateLeftBendLines()
	{
		double reqNumOfLins = (profwid / .750);
		Double perValue = reqNumOfLins;
		int roundVal= (int) Math.round(perValue);
		int smallfeatureid, bigfeatureid;
		int entid, cnt, m_indx;
 		int featurecount;

		DbEntity dbEntity= null;

		DbFeature bl_left_Small = new DbFeature();
		smallfeatureid = Portal.IntGet("id");
		bl_left_Small.Name("bl_left_Small");



		DbTool	Bendlines;
		Bendlines = new DbTool("Bendlines");
		Bendlines = Model.ToolGet("Bendlines");
		Bendlines.Color( DbEntity.RGB(233, 51, 255) );

		//left side chamfer
		Point bendlineA = new Point(len45 , len45 + notchwidth + notchwidth, 0. );
		Point bendlineB = new Point(bendlineA.X(), ptI.Y(), 0. );

		DbLine LineA = new DbLine( bendlineA, bendlineB);
		entid = Portal.IntGet("id");


		bl_left_Small.Append(LineA);

		//do the 90 degree
		DbFeature bl_left_Big = new DbFeature();
		bigfeatureid = Portal.IntGet("id");
		bl_left_Big.Name("bl_left_Big");
		Bendlines = Model.ToolGet("Bendlines");
		Bendlines.Color( DbEntity.RGB(233, 51, 255) );

		//left side chamfer
		Point bendlineC = new Point(len45 + notchwidth, len45 + notchwidth , 0. );
		Point bendlineD = new Point(bendlineC.X(), bendlineB.Y() + notchwidth, 0. );

		DbLine LineB  = new DbLine( bendlineC, bendlineD);
		entid = Portal.IntGet("id");
		LeftSmallBend = LineB.StartPt();


		bl_left_Big.Append(LineB);
		splitval = profwid;
		reqNumOfLins = (bendlineD.Y() - bendlineC.Y())/.75;

		perValue = reqNumOfLins;

		roundVal= (int) Math.round(reqNumOfLins);


		featurecount = bl_left_Big.Count();

	}

	private void CreateRightBendLines()
	{

		double reqNumOfLins = (profwid / .750);
		Double perValue = reqNumOfLins;
		int roundVal= (int) Math.round(perValue);
		int smallfeatureid, bigfeatureid;
		int entid, cnt, m_indx;
 		int featurecount;

		DbEntity dbEntity= null;

		DbFeature bl_Right_Small = new DbFeature();
		smallfeatureid = Portal.IntGet("id");
		bl_Right_Small.Name("bl_Right_Small");

		DbTool	Bendlines;
		Bendlines = new DbTool("Bendlines");
		Bendlines = Model.ToolGet("Bendlines");
		Bendlines.Color( DbEntity.RGB(233, 51, 255) );

		//left side chamfer
		Point bendlineA = new Point(ptS.X() , ptI.Y(), 0. );
		Point bendlineB = new Point(bendlineA.X(), ptF.Y(), 0. );

		DbLine LineA = new DbLine( bendlineA, bendlineB);
		entid = Portal.IntGet("id");

		RightMidPt= LineA.StartPt();
		bl_Right_Small.Append(LineA);
		splitval = profwid;

		//do the 90 degree

		DbFeature bl_Right_Big = new DbFeature();
		bigfeatureid = Portal.IntGet("id");
		bl_Right_Big.Name("bl_Right_Big");

		Bendlines = Model.ToolGet("Bendlines");
		Bendlines.Color( DbEntity.RGB(233, 51, 255) );

		//left side chamfer
		Point bendlineC = new Point(ptR.X(), len45 + notchwidth , 0. );
		Point bendlineD = new Point(bendlineC.X(), ptR.Y(), 0. );

		DbLine LineB  = new DbLine( bendlineC, bendlineD);
		entid = Portal.IntGet("id");


		bl_Right_Big.Append(LineB);
		splitval = profwid;
		reqNumOfLins = (bendlineD.Y() - bendlineC.Y())/.75;

		perValue = reqNumOfLins;

	}

	private void CreateBendlines ()
	{

			Msg.Diagnostic( "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%GoingInto CreateLeftBendLines");
		CreateLeftBendLines();
			Msg.Diagnostic( "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%GoingInto CreateLeftBendLines");

			Msg.Diagnostic( "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%GoingInto CreateBottomBendLines");
		CreateBottomBendLines();
			Msg.Diagnostic( "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%GoingInto CreateBottomBendLines");

			Msg.Diagnostic( "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%GoingInto CreateTopBendLines");
		CreateTopBendLines();
			Msg.Diagnostic( "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%We Left CreateTopBendLines");

			Msg.Diagnostic( "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%GoingInto CreateRightBendLines");
		CreateRightBendLines();
			Msg.Diagnostic( "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%We Left CreateRightBendLines");

	}

	private void UpdateCommandSize()
	{

		//Msg.Display("The Update Command Size Zone1ID is < " + Zone1ID  + " >");

		int indx =0;
		int count;
		DbCommand	dbCommand;

		Selector.Flush();

		count =(Model.EntityCount( Const.COMMAND ));

		dbCommand = Model.CommandGet( indx );
		dbCommand.IntSet( "label_size", CMD_size );



		Selector.Add( Model.EntityGet("PanelSize"));

		Portal.Execute("Feature:Modify: id=" + Zone1ID + ",selected=1");

	}


	private DbFeature WorkzoneGet()
	{
		return ( Model.FeatureGet("_repo_zone_2") );
	}


	private void MovePartToCenter()
	{
			LeftLN=Model.EntityGet("LeftLine");

			LeftName = DbLine.DbLine( LeftLN);

			LeftStartPt = LeftName.StartPt();

			LeftMidPt = LeftName.MidPt();

			LeftEndPt = LeftName.EndPt();

			TopLN=Model.EntityGet("TopLine");

			TopName = DbLine.DbLine( TopLN);

			TopMidPt = TopName.MidPt();

			BottomLN=Model.EntityGet("BottomLine");

			BottomName = DbLine.DbLine( BottomLN);
			BottomMidPt = BottomName.MidPt();


			Selector.All( true ); // enable selection of all entity types
			Selector.AddAll();

			//Msg.Display("Moving part to the center");
			Portal.Execute("transform:move: sx=" + TopMidPt.X() +",sy=" + LeftMidPt.Y() + ",sz=0,ex=" + (matlength/2) + ",ey=" + (matwidth/2) + ",ez=0,copies=0");

	}
	private DbFeature ZoneClone( DbFeature zone1 )
	{
		DbFeature	zone2;
		int			count, zonecolor;

		zone2 = new DbFeature();

		zonecolor = zone2.Color();

		zone2.StringSet( "Type", "_zone" );
		zone2.IntSet( "_zone_num", 2 );

		zone2.DoubleSet( "_zone_bottom", zone1.IntGet("_zone_bottom") );
		zone2.DoubleSet( "_zone_top", zone1.DoubleGet("_zone_top") );

		zone2.DoubleSet( "_zone_left", (RepoAmt * -1) );
		zone2.DoubleSet( "_zone_right", (zone1.DoubleGet("_zone_right") - RepoAmt) );
		zone2.DoubleSet( "_zone_repo", 0. );

		count = zone1.IntGet("_clamp_num");
		Portal.Execute("view:erase:id=");

		Portal.Execute("zone:update: id =" + zone2.Id()
					+ ",hold_type=0,_zone_num=2,_hold_x=0,_hold_y=0,_zone_repo=0");

		zone2.Color(zonecolor);
		Selector.AddAll();

		Portal.Execute("Feature:Modify: id=" + zone2.Id() + ",selected=1");

		Selector.Flush();

		return zone2;

	}

		Point ptA;
		Point ptB = null;
		Point ptC;
		Point ptD;
		Point ptE;
		Point ptF;
		Point ptG;
		Point ptH;
		Point ptI;
		Point ptJ;
		Point ptK;
		Point ptL;
		Point ptM;
		Point ptN;
		Point ptO;
		Point ptP;
		Point ptQ;
		Point ptR;
		Point ptS;
		Point ptT;
		Point ptU;
		Point ptV;
		Point ptW;
		Point ptX;
		Point ptY;
		Point ptZ;
		Point ptAA;
		Point ptAB;
		Point LeftStartPt;
		Point LeftMidPt;
		Point LeftEndPt;
		Point TopStartPt;
		Point TopMidPt;
		Point TopEndPt;
		Point RightStartPt;
		Point RightMidPt;
		Point RightEndPt;
		Point BottomStartPt;
		Point BottomMidPt;
		Point BottomEndPt;
		Point TempPT;
		Point ShakerTab1;
		Point ShakerTab2;
		Point ShakerTab3;
		Point ShakerTab4;
		Point ShakerTab5;
		Point ShakerTab6;
		Point ShakerTab7;
		Point ShakerTab8;

		DbProfile ShakerTabProf;

		double		zoneBottom;
		double		zoneLeft;
		double		zoneRight;
		double		zoneTop;

		int			zoneNum;
		int			Zone1ID;
		int			Zone2ID;
		int			Zone3ID;
		int			Zone4ID;
		int			Logo;
		int			Sqr1000_Hole1;
		int			Sqr1000_Hole2;
		int			Sqr1000_Hole3;
		int			Sqr1000_Hole4;
		int			Sqr1000_Hole5;
		int			Sqr1000_Hole6;
		int			Sqr1000_Hole7;
		int			Sqr1000_Hole8;
		int			Sqr1000_Hole9;
		int			Sqr1000_Hole10;
		int			Sqr1000_Hole11;
		int			Sqr1000_Hole12;
		int			Sqr1000_Hole13;
		int			Sqr1000_Hole14;
		int			Sqr1000_Hole15;
		int			Sqr1000_Hole16;
		int			Sqr1000_Hole17;
		int			Sqr1000_Hole18;
		int			Sqr1000_Hole19;
		int			Sqr1000_Hole20;
		int			Sqr1000_Hole21;
		int			Sqr1000_Hole22;
		int			Sqr1000_Hole23;
		int			Sqr1000_Hole24;
		int			Sqr1000_Hole25;
		int			Sqr1000_Hole26;
		int			Sqr1000_Hole27;
		int			Sqr1000_Hole28;
		int			Sqr1000_Hole29;
		int 		Zone1HoleID;
		int 		nTheCMD;
		int 		GrowId;
		int			m_active_workzoneID;
		int 		first45Toolpath;
		int 		second45Toolpath;
		int 		third45Toolpath;
		int 		fourth45Toolpath;
		int			Cmd_ID;
		int			BS_ID;


		DbFeature 	LeftTrim1 = null;
		DbFeature 	LeftTrim2 = null;
		DbFeature 	TopTrim = null;
		DbFeature 	RightTrim = null;
		DbFeature 	BottomTrim = null;
		DbEntity 	WZ = null;

		DbCommand 	TheCMD = null;

		boolean 	CreatedZone2;
		boolean DoesIterfer;
		boolean bCreatedBottom;
		DbEntity HoleToCheck;

		DbFeature L_Trim1;
		DbEntity L_Trim2;

		DbEntity T_Trim1;
		DbEntity T_Trim2;
		DbEntity T_Trim3;

		DbEntity R_Trim1;
		DbEntity R_Trim2;

		DbEntity B_Trim1;
		DbEntity B_Trim2;
		DbEntity B_Trim3;



}
