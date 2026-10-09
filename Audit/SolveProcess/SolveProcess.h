// SolveProcess.h : main header file for the SOLVEPROCESS DLL
//

#if !defined(AFX_SOLVEPROCESS_H__F90E0226_1A5A_11D3_B7F1_000039A6570C__INCLUDED_)
#define AFX_SOLVEPROCESS_H__F90E0226_1A5A_11D3_B7F1_000039A6570C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

#include "Command.h"
#include "RouteList.h"

#include "DbLine.h"
#include "DbArc.h"
#include "DbCurveList.h"

class CInt2d;

/////////////////////////////////////////////////////////////////////////////
// CSolveProcessApp
// See SolveProcess.cpp for the implementation of this class
//

class CSolveProcessApp : public CWinApp
{
public:
	CSolveProcessApp();

	static CReturn dllExport RegisterProcess( CRouteList* io_route );

	static CReturn dllExport Solve( CCommand* io_cmd );

	static CReturn dllExport ArcSEP( CCommand* io_cmd );
	static CReturn dllExport ArcSCI( CCommand* io_cmd );
	static CReturn dllExport ArcSER( CCommand* io_cmd );
	static CReturn dllExport ArcPTR( CCommand* io_cmd );
	static CReturn dllExport ArcTT( CCommand* io_cmd );
	static CReturn dllExport ArcCR( CCommand* io_cmd );
	static CReturn dllExport ArcPAL( CCommand* io_cmd );
#if (_CI)
	static CReturn dllExport ArcPTR_CI( CCommand* io_cmd );
#endif
	static CReturn dllExport ArcLLA( CCommand* io_cmd );

	// Lives in SolveProcess2.cpp
	static CReturn dllExport ArcAngles( CCommand* io_cmd );

	static CReturn dllExport LineSAL( CCommand* io_cmd );
	static CReturn dllExport LineST( CCommand* io_cmd );
	static CReturn dllExport LineTT( CCommand* io_cmd );

	static CReturn dllExport Intersect( CCommand* io_cmd );
	static CReturn dllExport TrimExtend( CCommand* io_cmd );
	static CReturn dllExport Split( CCommand* io_cmd );
	static CReturn dllExport Chamfer( CCommand* io_cmd );
	static CReturn dllExport Blend( CCommand* io_cmd );

	static CReturn ChainCut( CCommand* io_cmd );

	static CReturn dllExport Evaluate( CCommand* io_cmd );

	static CReturn dllExport StartTan( CCommand* io_cmd );
	static CReturn dllExport EndTan( CCommand* io_cmd );
	static CReturn dllExport StartPt( CCommand* io_cmd );
	static CReturn dllExport EndPt( CCommand* io_cmd );
	static CReturn dllExport Length( CCommand* io_cmd );
	static CReturn dllExport CenterPt( CCommand* io_cmd );
	static CReturn dllExport Radius( CCommand* io_cmd );
	static CReturn dllExport Invert( CCommand* io_cmd );

	// These three live in SolveProcess2.cpp
	static CReturn dllExport Dir( CCommand* io_cmd );
	static CReturn dllExport Area( CCommand* io_cmd );
	static CReturn dllExport Reduce( CCommand* io_cmd );

	static CReturn dllExport PointClosest( CCommand* io_cmd );

	static CReturn dllExport MultiIntersect( CCommand* io_cmd );
	static CReturn dllExport MultiIntersectCount( CCommand* io_cmd );
	static CReturn dllExport MultiIntersectGet( CCommand* io_cmd );
	static CReturn dllExport MultiIntersectFlush( CCommand* io_cmd );

private:

	static CReturn arc_ter_line( CCommand* io_cmd, CDbLine* in_line, bool in_end );
	static CReturn arc_ter_arc( CCommand* io_cmd, CDbArc* in_line, bool in_end );

	static CDbCurve* CurveGet( const CModel& model, ID id );
	static void CurvesGet( const CModel& model, ID id, CDbCurveList* dbCurves );

	static void GetCandidates(
						const CGeoCurve&	geoCurve,
						double				uparam,
						const C2dCoord&		intsct,
						int					intnum,
						CGeoCurveArray*		candidates );

	static CString ResultFormat( ID id, CGeoCurve* geoCurve );

	static void GetDefaultSolutions(
						const CGeoCurveArray&	candidatesA,
						const CGeoCurveArray&	candidatesB,
						const CInt2d&			int2d,
						const C3dCoord&			pickPtA,
						const C3dCoord&			pickPtB,
						int*					indxA,
						int*					indxB );

	static int FindDefaultSolution(
						const CGeoCurveArray&	candidates,
						const C3dCoord&			pickPt,
						const C2dCoord&			intsct,
						double					uparam );

	static int FindDefaultSolution(
						const CGeoCurveArray&	candidates,
						const CString&			action );

	static int FindBestIntersection(
						const CInt2d&	int2d,
						const C3dCoord&	pickPtA, 
						const C3dCoord& pickPtB );

	static CReturn ProfileReduce( CDbProfile* dbProfile, double tol );

	static void ArcInvert( CGeoArc* geoArc );

public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSolveProcessApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CSolveProcessApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:  // Data

	static CRouteList m_solveRouter;;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SOLVEPROCESS_H__F90E0226_1A5A_11D3_B7F1_000039A6570C__INCLUDED_)
