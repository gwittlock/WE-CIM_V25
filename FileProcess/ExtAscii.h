
#ifndef _EXTASCII_H
#define _EXTASCII_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef _IMPORTUTIL_H
#include "ImportUtil.h"
#endif

#include "text_file.h"

#include "Return.h"
#include "3dCoord.h"

#include "DbCurve.h"

#include "Model.h"

#include "DaoDB.h"

class CAutoModDb;


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class CExtAscii
{
public:

	CExtAscii();

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Add the entities in the named file to the given model.
	// Entities are filtered using the given database.
	CReturn Read(
				const CString&	ascFileName,
				CAutoModDb*		autoModDb,
				CModel*			model,
				bool			buildToolSetup );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Build a list of layer names from the entity data.
	CReturn Analyze( const CString& ascFileName, const CAutoModDb& autoModDb );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Write the layer names to a file.
	CReturn AnalysisWrite( const CString& anlFilePath );

	virtual ~CExtAscii();

protected:

private:  // Disabled.

	CExtAscii( const CExtAscii& );
	const CExtAscii& operator = ( const CExtAscii& );
	int operator == ( const CExtAscii& ) const;
	int operator != ( const CExtAscii& ) const;

private:  // Methods

	int Parse(  const CString& buf, CStringArray* fields );

	void LayerAdd( const CString& buf );
	CString LayerNameCreate( const CStringArray& fields );

	CString OffsetDir( const CString& buf );
	int ToolFind( const CStringArray& fields );

	CReturn StockAndPlanesCreate( const CStringArray& fields );
	CReturn EntityCreate( const CString& buf );
	CReturn LineCreate( const CStringArray& fields, const CString& layerName );
	CReturn ArcCreate( const CStringArray& fields, const CString& layerName );
	CReturn CircleCreate( const CStringArray& fields, const CString& layerName );
	CReturn HoleCreate( const CStringArray& fields, const CString& layerName );

	CDbTool* DbLayer( const CString& layerName );
	CDbWorkplane* DbWork( const CString& planeName );

private:

	CTextFile	m_file;			// the asc file object
	CImportUtil	m_importUtil;
	CAutoModDb*	m_autoModDb;	// the machine database
	CModel*		m_model;		// the recipient model

	int m_lineNo;  // for debugging
};

#endif
