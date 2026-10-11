#ifndef _MYPM4_H
#define _MYPM4_H

// ============================================================================
//		PM4 File Interface
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
#include "Jof.h"

class CAutoModDb;

// =======================================================================
//		Application Type...  affects hole interpretation
//
enum ePM4App
{
	PM4_MILL = 0,
	PM4_FAB = 1
};

// =======================================================================
//		Tag and Size definitions...
//

#define PM4_POINT_TAG		0x01
#define PM4_LINE_TAG		0x02
#define PM4_ARC_TAG			0x03
#define PM4_USER_TAG		0x04
#define PM4_POLY_TAG		0x0a
#define PM4_SUBCALL_TAG		0x0e
#define PM4_TEXT_TAG		0x10

#define PM4_SUBDEF_TAG		0x29
#define PM4_MAP_TAG			0x64
#define PM4_NAME_TAG		0x65
#define PM4_VIEW_TAG		0x66
#define PM4_MATRIX_TAG		0x69


#define PM4_COM_SIZE		20
#define PM4_POINT_SIZE		44
#define PM4_LINE_SIZE		68
#define PM4_ARC_SIZE		102
#define PM4_POLY_PT_COUNT	92
#define PM4_POLY_PT_BASE	98
#define PM4_USER_SIZE		120
#define PM4_SUBCALL_SIZE	160
#define PM4_TEXT_HEAD		64		// Pre-text buffer size
#define PM4_TEXT_TAIL		46		// Post-text buffer size
#define PM4_TEXT_SIZE		(PM4_TEXT_HEAD+PM4_TEXT_TAIL)
#define PM4_SUBDEF_SIZE		16
#define PM4_VIEW_SIZE		96		//	Need Sixteen copies of this
#define PM4_MATRIX_SIZE		182


// =======================================================================
// HEADER[1024]:
//
//		Offst		Byte		Descrip
//		-----		-----		------------------------
//		0x0000	0x0080	Display text
//		0x0080	0x0010	Version text
//		0x0090	0x0010	JobPlane text
//		0x00a0	0x0100	16x Window Extents @ float Min X,Y, Max X,Y
//		0x01a0	0x00a0	16x Window Names @ char[10]
//		0x0240	0x00a0	16x Group Names @ char[10]
//		0x02e0	0x00c8	100x Layer definition @ int16  (11:color, 4:style, 1:mask)
//		0x03a8	0x0002	Active ref plane @ int16
//		0x03aa	0x0018	Base Envelope @ float Min X,Y,Z, Max X,Y,Z
//		0x03c2	0x0006	3x Tool Plane @ int16
//		0x03c8	0x0038	(padding to suit)
//		0x0400
//

#if ORIGINAL_CODE
#define PM4_HEADER_LEN	0x0431	// Includes origin
#else
#define PM4_HEADER_LEN	1024
#endif
#define PM4_TRAILER_LEN	0x0826

#define PM4_OFF_DISPLAY 0x0000
#define PM4_OFF_VERSION	0x0080
#define PM4_OFF_JOBPLAN	0x0090
#define PM4_OFF_WINEXT	0x00a0
#define PM4_OFF_WINNAME	0x01a0
#define PM4_OFF_GRPNAME	0x0240
#define PM4_OFF_LAYER	0x02e0
#define PM4_OFF_ACTIVE	0x03a8
#define PM4_OFF_BASE		0x03aa
#define PM4_OFF_TPLANE	0x03c2
#define PM4_OFF_PADDING	0x03c8
#define PM4_OFF_ORIGIN	0x0400

// ============================================================================

class CMyPm4
{
public:

	CMyPm4( void );

	void SetJof( const CJof& jof )	{ m_jof = (const CJof*) &jof; }

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Read the named pm4 file into the given model.
	// workPlaneDef -- (0) router / (1) point-to-point
	CReturn Read(
				const CString&	in_filename,
				const CString& in_stockname,
				CModel*			model,
				CAutoModDb*		autoModDb );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// These methods are used by the Configuration
	// Manager to extract layer names.
	CReturn Analyze( const CString& dxfFileName );

	int LayerCount();
	CString LayerName( int index );

	void			Stock( const CString& stock )		{ m_stock = stock; }
	const CString&	Stock( void ) const					{ return m_stock; }

	~CMyPm4( void );

private:  // Methods

	CReturn Read( 
				CModel*			model, 
				CAutoModDb*		autoModDb, 
				const CString& in_stockname );

	// Read methods... implemented in pm4_read.cpp
	int		do_read_codesize( BYTE code );
	CReturn	do_read_refplane( CModel* model );
	CReturn	do_read_geometry( CModel* model );
	CReturn do_read_layer( void );

	CString	read_layername( BYTE* in_pool );
	CReturn	read_common( CModel* model, BYTE* in_pool, const CString& in_layer, CDbEntity* io_elem );

	CReturn	read_point( CModel* model,BYTE* in_pool );
	CReturn	read_line( CModel* model,BYTE* in_pool );
	CReturn	read_arc( CModel* model,BYTE* in_pool );
	CReturn	adjust_arc( C3dCoord& st, C3dCoord& en, C3dCoord* ct );

	CReturn	read_user( CModel* model, BYTE* in_pool );
	CReturn read_poly( CModel* model, BYTE* io_pool );
//	CReturn	read_subcall( BYTE* in_pool );
//	CReturn	read_text( BYTE* in_pool );
//	int		read_subdef( BYTE* in_pool );
	CReturn	read_refplane( CModel* model, BYTE* in_pool );

	int		get2int( BYTE *in_pool );
	int		get4int( BYTE *in_pool );
	float	get4float( BYTE *in_pool );
	double	get8double( BYTE *in_pool );
	void	get24point( BYTE* in_pool, C3dCoord* io_point );
	CString	getNascii( BYTE *in_pool, int in_num );
	void	getNbin( BYTE *in_pool, BYTE *out_buf, int in_num );

	CReturn PostProcess();

	double CheckGet( BYTE *in_pool );
	double ClearGet( BYTE *in_pool );

	CDbTool* ToolFind( const CModel& model, const CString& name );

	CDbTool* GetTool( const CString& in_tool );

	void XY_PlaneCreate();

private:  // Data.
	CFile		m_file;			// the pm4 file object
	CImportUtil	m_importUtil;
	CAutoModDb* m_autoModDb;	// the machine database
	CModel*		m_model;		// the recipient model

	int			m_orderid;

	CString		m_stock;

	CStringArray	m_refname;	// Names of reference planes, in creation order

	const CJof*	m_jof;

	bool		m_didReadOrigin;
};


#endif
