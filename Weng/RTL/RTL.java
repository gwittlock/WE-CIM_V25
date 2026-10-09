
package Weng.Rtl;

import Weng.System.*;


/**
 * Use this singleton class to ....
 */
public class RTL
{
	static private final boolean DEBUG = false;

	static public void StandardAnalysis( boolean keepIntermediateFiles )
	{
		if ( IsActive() )
		{
			String currSrcPath = RTL.CurrentSourceFilePath();
			String resultDir   = RTL.CurrentResultDir();
			String fileName    = RTL.FileNameNoExt( currSrcPath );

			String baselineMM2 = RTL.CurrentBaselineDir() + "\\" + fileName + ".mm2";
			String resultMM2   = resultDir + "\\" + fileName + ".mm2";
			String diffFile    = resultDir + "\\" + fileName + ".dif";

			RTL.ExportMM2( resultMM2 );

			if (DEBUG)
			{
				Msg.Diagnostic(  "baselineMM2 <" + baselineMM2 + ">\n"
								+ "resultMM2 <" + resultMM2 + ">\n"
								+ "diffFile <" + diffFile + ">" );
			}

			int diffSize = RTL.Diff( baselineMM2, resultMM2, diffFile, 0, keepIntermediateFiles );

			if (diffSize > 0)
				Msg.Diagnostic( "   differences in " + diffFile );

			if ( !keepIntermediateFiles )
				FileMgr.Delete( resultMM2 );
		}
	}

	static public boolean IsActive()
	{
		String cmd = "Admin:RTL:state:";
		return (Portal.Execute( cmd ) ? (Portal.IntGet( "active" ) == 1) : false);
	}

	static public String SourceRootDir()
	{
		return ( VarSys.StrGet( "$RTLSOURCE" ) );
	}

	static public String ResultsRootDir()
	{
		
		return ( VarSys.StrGet( "$RTLRESULTS" ) );
	}

	static public String CurrentSourceFilePath()
	{
		return ( VarSys.StrGet( "javafile" ) );
	}

	static public String CurrentBaselineDir()
	{
		String srcCurrFile = CurrentSourceFilePath();

		int indx = srcCurrFile.lastIndexOf( '\\' );

		String baselineDir = 
			((indx >= 0) ? srcCurrFile.substring( 0, indx ) : srcCurrFile) + "\\Baseline";

		return baselineDir;
	}

	static public String CurrentResultDir()
	{
		String srcRootDir  = SourceRootDir();
		String srcCurrFile = CurrentSourceFilePath();

		String rsltDir = ResultsRootDir();

		if (DEBUG)
		{
			Msg.Diagnostic( "RTL::CurrentResultDir()\n"
							+ "    srcRootDir <" + srcRootDir + "> \n"
							+ "    srcCurrFile <" + srcCurrFile + "> \n"
							+ "    rsltDir <" + rsltDir + ">" );
		}

		String relative = srcCurrFile.substring( srcRootDir.length() );
		int indx = relative.lastIndexOf( '\\' );
		if (indx > 0)
		{
			rsltDir += relative.substring( 0, indx );
		}

		if (DEBUG)
		{
			Msg.Diagnostic( "RTL::CurrentResultDir()\n"
							+ "    final rsltDir <" + rsltDir + ">" );
		}

		return rsltDir;
	}

	static public String FileName( String filepath )
	{
		int indx = filepath.lastIndexOf( '\\' );

		if (indx < 0)
			indx = -1;

		return (filepath.substring( (indx+1), (filepath.length() - 1) ));
	}

	static public String FileNameNoExt( String filepath )
	{
		String filename = FileName( filepath );

		int indx = filename.lastIndexOf( '.' );

		if (indx < 0)
			indx = filename.length() - 1;

		return (filename.substring( 0, indx ));
	}

	static public boolean ExportMM2( String path )
	{
		String cmd = "File:ExportMM2:"
					 + " file=\"" + path + "\""
					 + ",rtl=1";

		boolean okay = Portal.Execute( cmd );

		return okay;
	}

	/**
	 *    where:
	 *
	 *       type (0) text-based comparison with inline entity explosion (default)
	 *
	 *       keep (0) keep the difference file when not empty (default)
	 *            (1) same as (0) but additionally keeps intermediate files
	 */
	static public int Diff(
							String	sourceMM2FilePath,
							String	resultMM2FilePath,
							String	diffFilePath,
							int		type,
							boolean	keepIntermediateFiles )
	{
		String cmd;
		String intFilePathA;
		String intFilePathB;
		int indx;
		int len = 0;

		indx = sourceMM2FilePath.lastIndexOf( '.' );
		intFilePathA = sourceMM2FilePath.substring( 0, indx ) + ".txt";

		cmd = "File:MM2Dump:"
			  + " mm2=\"" + sourceMM2FilePath + "\""
			  + ",txt=\"" + intFilePathA + "\""
			  + ",type=" + type;

		Portal.Execute( cmd );

		indx = resultMM2FilePath.lastIndexOf( '.' );
		intFilePathB = resultMM2FilePath.substring( 0, indx ) + ".txt";

		cmd = "File:MM2Dump:"
			  + " mm2=\"" + resultMM2FilePath + "\""
			  + ",txt=\"" + intFilePathB + "\""
			  + ",type=" + type;

		Portal.Execute( cmd );

		try
		{
			boolean diffs = DiffPrint.Compare(
								intFilePathA, intFilePathB, diffFilePath, false );

			if ( diffs )
			{
				len = FileMgr.Length( diffFilePath );
				if (len == 0)
				{
					FileMgr.Delete( diffFilePath );
				}
			}
		}
		catch (Exception e)
		{
		}
		
		if (!keepIntermediateFiles || len == 0)
		{
			// Get rid of the input (model.txt) files
			FileMgr.Delete( intFilePathA );
			FileMgr.Delete( intFilePathB );
		}

		return len;
	}
}

