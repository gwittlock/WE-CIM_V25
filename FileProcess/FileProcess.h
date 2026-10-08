// FileProcess.h : main header file for the FILEPROCESS DLL
//

#if !defined(AFX_FILEPROCESS_H__7F743DE5_3B60_11D3_B7F1_000039A6570C__INCLUDED_)
#define AFX_FILEPROCESS_H__7F743DE5_3B60_11D3_B7F1_000039A6570C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"		// main symbols

#include "Command.h"
#include "RouteList.h"

/////////////////////////////////////////////////////////////////////////////
// CFileProcessApp
// See FileProcess.cpp for the implementation of this class
//

class CFileProcessApp
{
public:

	// CFileProcessApp();

	static CReturn dllExport RegisterProcess( CRouteList* io_route );

	static CReturn dllExport File( CCommand* io_cmd );

	static CReturn dllExport ExportMM2( CCommand* io_cmd );
	static CReturn dllExport ImportMM2( CCommand* io_cmd );
	static CReturn dllExport MergeMM2( CCommand* io_cmd );

	static CReturn dllExport ExportXML( CCommand* io_cmd );
	static CReturn dllExport ImportXML( CCommand* io_cmd );

	static CReturn dllExport ExportMacro( CCommand* io_cmd );

	static CReturn dllExport ImportDXF( CCommand* io_cmd );
	static CReturn dllExport AnalyzeDXF( CCommand* io_cmd );
	static CReturn dllExport ExportDXF( CCommand* io_cmd );

	static CReturn dllExport ImportPM4( CCommand* io_cmd );
	static CReturn dllExport AnalyzePM4( CCommand* io_cmd );

	static CReturn dllExport AnalyzeJOF( CCommand* io_cmd );

	static CReturn dllExport ImportExtAscii( CCommand* io_cmd );
	static CReturn dllExport AnalyzeExtAscii( CCommand* io_cmd );

	static CReturn dllExport ImportIGES( CCommand* io_cmd );
	static CReturn dllExport AnalyzeIGES( CCommand* io_cmd );

	static CReturn dllExport TokenValue( CCommand* io_cmd );

	// Dump the model contents as either html, or text.
	static CReturn dllExport ModelDump( CCommand* io_cmd );

	// Reads an MM2 into a separate model and dumps as text.
	static CReturn dllExport MM2Dump( CCommand* io_cmd );

private:

	static CReturn MacroDump( CModel& model, CString filename );

	static bool DirectoryCreate( const CString& dir );

private:
	
	static CRouteList m_fileRouter;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILEPROCESS_H__7F743DE5_3B60_11D3_B7F1_000039A6570C__INCLUDED_)
