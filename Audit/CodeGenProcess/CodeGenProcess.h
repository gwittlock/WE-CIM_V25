// CodeGenProcess.h : main header file for the CODEGENPROCESS DLL
//

#if !defined(AFX_CODEGENPROCESS_H__EE20D1F1_2251_11D3_B7F1_000039A6570C__INCLUDED_)
#define AFX_CODEGENPROCESS_H__EE20D1F1_2251_11D3_B7F1_000039A6570C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

#include "Command.h"
#include "RouteList.h"
#include "StrgList.h"

class EntityCopier;
class CDbPattern;
class CDbFeature;
class CModel;
class CModelClfile;

enum ERapidType
{
	RAPID_NONE =		0,
	RAPID_IMPLICIT =	1,
	RAPID_EXPLICIT =	2
};

/////////////////////////////////////////////////////////////////////////////
// CCodeGenProcessApp
// See CodeGenProcess.cpp for the implementation of this class
//
class CCodeFileRec
{
public:

	CCodeFileRec( const CString& filepath, int sheet_num )
	{
		m_filepath = filepath;
		m_sheet_num = sheet_num;
	}

	~CCodeFileRec()  {  }

	const CString& FilePath() const  { return m_filepath; }

	int SheetNum() const  { return m_sheet_num; }

private:  // disabled

	CCodeFileRec();

private:

	CString	m_filepath;
	int	m_sheet_num;
};

typedef CDynamicArray<CCodeFileRec*> TCodeFileArray;



class CCodeGenProcessApp : public CWinApp
{
public:

	CCodeGenProcessApp();

	static CReturn dllExport RegisterProcess( CRouteList* io_route );

	static CReturn dllExport CodeGen( CCommand* io_cmd );

	static CReturn dllExport XrefCreate( CCommand* io_cmd );
	static CReturn dllExport XrefDestroy( CCommand* io_cmd );
	static CReturn dllExport XrefCount( CCommand* io_cmd );
	static CReturn dllExport XrefFetch( CCommand* io_cmd );
	static CReturn dllExport XrefRecNo( CCommand* io_cmd );

	static CReturn dllExport Highlight( CCommand* io_cmd );
	static CReturn dllExport Generate( CCommand* io_cmd );

	static CReturn dllExport Optimize( CCommand* io_cmd );

	static bool IsValidInstance( const CDbCommand& dbCommand );

	static CReturn dllExport WorkZonesVerify( CCommand* io_cmd );
	static CReturn dllExport ShowRapid( CCommand* io_cmd );

private:

	static CReturn StandardGenerate( CCommand* io_cmd );
	static CReturn CcsFabGenerate( CCommand* io_cmd );
	static CReturn SmcCodeGenerate( CCommand* io_cmd );

	static CReturn OptimizeAll( CCommand* io_cmd );
	static CReturn OptimizeSubset( CCommand* io_cmd );

	static CDbPattern*	FilesMerge(
								const CString&	pdbPath,
								CModel*			model );

	static CReturn	CcsFabFilesGet(
		const CString&	txtPath,
		TCodeFileArray*	fileList );

	static CReturn	FilesGet(
		const CString&	pdbPath,
		int				sheet_num,
		TCodeFileArray*	fileList );

	static CReturn	ReferenceEntitiesCopy( CModel& source, CEntityCopier* copier );

	static CDbPattern* SheetCreate(
								CModel*	model,
								int		sheetNo );

	static CDbPattern*	ModelCopy(
								CModel*	srcModel,
								CModel*	cpyModel,
								int		sheetNo,
								bool	copyCommon );
	static void StandardModelCopy(
						CModel*			srcModel,
						CModel*			cpyModel,
						CEntityCopier*	copier,
						CDbPattern*		sheet );

	static void SpecialModelCopy(
						CModel*			srcModel,
						CModel*			cpyModel,
						CEntityCopier*	copier,
						CDbPattern*		sheet );

	static void CodeViewerInit( CCommand* io_cmd, CModelClfile* clfile );

	static CReturn ConditionalCompile( const CString& cgFilePath );

	static ERapidType CanDisplayRapid(
		const CCodeGeoRec*	recA,
		const CCodeGeoRec*	recB,
		C3dCoord*			ps,
		C3dCoord*			pe );

	static bool IsRapid( const CGeoElem* elem );
	static bool StartPtGet( const CCodeGeoRec& rec, C3dCoord* pt );
	static bool EndPtGet( const CCodeGeoRec& rec, C3dCoord* pt );

	static void CodingParametersCopy( const CVarList& params, CModel* model );

	static void DropDoorDraw( const CGeoElem* elem, CCommand* io_cmd );
	static void DropDoorErase( CCommand* io_cmd );

	static CString NcPathFormat( int ccs, const CString& ncPath, const CString& nstPath);

	static void ModelDiffsCopy( const CModel& modelA, CModel* modelB );

private:	// data

	static CRouteList	m_codegenRouter;
	static CGeoPoly*	m_door_poly;

	static bool	m_can_draw_handles;

private:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCodeGenProcessApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CCodeGenProcessApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CODEGENPROCESS_H__EE20D1F1_2251_11D3_B7F1_000039A6570C__INCLUDED_)
