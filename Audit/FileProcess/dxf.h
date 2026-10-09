#ifndef _DXF_H
#define _DXF_H

// ============================================================================
//		DXF File Interface
//
// ============================================================================

#include "stdafx.h"
#include <afx.h>

#ifndef _IMPORTUTIL_H
#include "ImportUtil.h"
#endif

#include "text_file.h"

#include "Return.h"
#include "3dCoord.h"

#include "DbCurve.h"

#include "Model.h"

#include "DaoDB.h"
#include "CadLayer.h"
#include "CadEntity.h"

class CAutoModDb;
class CGeoArc;


// ============================================================================

#define LAYER_TABLE_SETUP		0
#define LAYER_TABLE_MAP			1

enum eDxfMapMode
{
	DxfMapIgnore	= 0,
	DxfMapAssign	= 1,
	DxfMapParse		= 2
};

#define DXF_MAP_ZLEVEL			0x0001
#define DXF_MAP_PROFTOP			0x0002
#define DXF_MAP_OFFSET			0x0004
#define DXF_MAP_CLEARZ			0x0008

#define OCS_XY_POS	0
#define OCS_XY_NEG	1
#define OCS_YZ_POS	2
#define OCS_YZ_NEG	3
#define OCS_ZX_POS	4
#define OCS_ZX_NEG	5

// ============================================================================

class CDxf
{
public:

	CDxf( void );

	CReturn Read(
				const CString&			dxfpath,
				const CCadLayerList&	acceptedLayers,
				CAutoModDb*				autoModDb );
	
	const CCadEntityList&	Entities() const		{ return m_entities; }

	CReturn ReadPostProc(
					CModel*		model,
					CAutoModDb*	autoModDb,
					double		rotation );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// These methods are used by the Configuration
	// Manager to extract layer names.
	CReturn Analyze( const CString& dxfFileName );
	int LayerCount();
	CString LayerName( int index );

	~CDxf( void );

private:  // Methods

	void	AcceptedLayersInit( const CCadLayerList& acceptedLayers );

	CReturn	ReadEntities( const CString& dxfpath );

#if REQUIRED
	CReturn Read( CModel* model, CAutoModDb* autoModDb );
#endif
	// Methods... implemented in dxf_read.cpp
	CReturn	read_pair( void );
	CReturn	read_section( void );
	CReturn	read_header( void );
	CReturn	read_ucs( void );
	CReturn	read_tables( void );
	CReturn	read_one_table( void );
	CReturn	read_layers( void );
	CReturn	read_entities( const CString& terminal );
	bool	parse_common( void );
	CReturn	read_line( void );
	CReturn	read_polyline( void );
	CReturn	read_vertex( C3dCoord* vertex, double* inc_ang );
	CReturn	read_arc( void );
	CReturn	read_circle( void );
	CReturn	read_point( void );
	CReturn	read_text( void );

	CReturn read_layer_table();

//	void	parse_comment( CString in_comment );

	CReturn PostProcess();

	CReturn LineCreate( const C3dCoord& ps, const C3dCoord& pe );

	CGeoArc* AcadArc( const C3dCoord& ps, const C3dCoord& pe, double inc_ang );

	// TODO: Analyze methods... implemented in dxf_analyze.cpp
//	bool		analyze_pair( void );
//	bool		analyze_section( void );
//	bool		analyze_entities( void );
//	bool		analyze_one( void );
	// -------------------

private:  // Data.

	CTextFile	m_file;			// the dxf file object
	CImportUtil	m_importUtil;
	CAutoModDb* m_autoModDb;	// the machine database
	CModel*		m_model;		// the recipient model

	int			m_group;		// the current data tag obtained by pair_read()
	CString		m_value;		// the current data value obtained by pair_read()

	CString		m_layer;		// the current layer obtained by parse_common()
	int			m_lineNo;		// for debugging

	CCadLayerList	m_acceptedLayers;
	CCadEntityList	m_entities;
	C3dVec			m_normal;

	double		m_xorg;
	double		m_yorg;
	double		m_zorg;
};


#endif
