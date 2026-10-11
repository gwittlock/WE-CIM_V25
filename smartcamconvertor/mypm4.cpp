// ============================================================================
//		PM4 File Interface
//
// ============================================================================

#include <stdafx.h>

#include "cmn_resource.h"
#include "MathConst.h"
#include "StringConst.h"

#include "DbEntity.h"
#include "DbPoint.h"
#include "DbWorkplane.h"
#include "DbIterator.h"
#include "AutoModDb.h"
#include "AutoMod.h"
#include "MyPm4.h"


// ============================================================================

CMyPm4::CMyPm4( void )
	: m_importUtil(),
	  m_model( NULL ),
	  m_autoModDb( NULL ),
	  m_jof( NULL )
{
	m_didReadOrigin = false;
}

CMyPm4::~CMyPm4( void )
{
}


// ============================================================================
//		Read
//
//		Read and translate the data file
//
CReturn
CMyPm4::Read(
	const CString&	in_filename,
	const CString& in_stockname,
	CModel*			model,
	CAutoModDb*		autoModDb )
{
	CFileException	err;
	CReturn			ret;

	if (model == NULL)
	{
		ret.Internal( IDS_INTERNAL_ERROR, "CMyPm4::Read() no model" );
		return ret;
	}

	if (m_file.Open( in_filename, CFile::modeRead ) )
	{
		ret += Read( model, autoModDb, in_stockname );
		m_file.Close();

		m_model->PostReadInit();
	}
	else
		ret.SystemError();

	return ret;
}

// Temporary 'native workplanes' are created.
// The smartcam entities are created on these workplanes.
// The entities are then tranformed and assigned to the target workplanes.
// The temporary workplanes are destroyed.
//
CReturn
CMyPm4::Read( 
	CModel*			model, 
	CAutoModDb*		autoModDb,
	const CString& in_stockname	)
{
	CReturn status;

	double zstock;

	double thickness = autoModDb->MaterialThick();
	if (EQUAL(thickness, UNDEFINED))
		thickness = 0.1;

	// BYPASS autoModDb->WorkplaneType() because it requires a valid tool setup id.
	// int workType = autoModDb->WorkplaneType();
	int workType = autoModDb->MachineAttributes().getInt( "Workplane_Type_ID", 1 );
	int machType = autoModDb->MachineType();

	m_model = model;
	m_autoModDb = autoModDb;

	Stock( in_stockname );

	m_importUtil.Init( m_model, m_autoModDb );

	status += do_read_refplane( model );
	status += do_read_geometry( model );

// ... read the damn thing

	// --------------------------------------------------------------

	C3dBox	box;
	if ( !status.isError() )
	{
		int dir = m_importUtil.StockDirection();
		status += m_importUtil.HeaderUpdate( m_refname[0], dir, thickness, &box, &zstock );
	}

	if ( status.IsOk() )
		status = m_importUtil.ViewPlanesCreate( box.Dx(), box.Dy(), thickness );

	if ( !status.isError() )
		status += m_importUtil.StandardPlanesCreate( box.Dx(), box.Dy(), thickness, workType, machType );

	// Per Gary's instructions, we want to use the SmartCAM coordinates verbatim
	// because many people to single parts, and they want their output to match.
	if ( !status.isError() )
	{
		// 10 Dec 2003 eww.  Note that passing dx 0, dy 0 on Y- workplanes (type 2) causes
		//	the Top plane to transform up into the correct position, but ALSO forces the 
		//	geometry down by this same distance -- forcing it to be too low in world.
		//	However, by passing box.Dy() for type 2 planes, we nullify the geometry transform,
		//	leaving it in place, which is correct *IF* the geometry was correctly negative
		//	to begin with.
		//	Since I *don't* know what will happen to type 1 planes, but I suspect this change
		//	would shift the geometry in the wrong direction, I leave dy=0 for the type 1 case.
		if (workType == 2)
		{ status = m_importUtil.SmartCAMPlanesTransform( m_refname, 0., box.Dy(), thickness ); }
		else
		{ status = m_importUtil.SmartCAMPlanesTransform( m_refname, 0., 0., thickness ); }
	}

#if REQUIRED
	// Profiling is handled later by Pm4JofConvertor.cpp
	if ( status.IsOk() )
		status = m_importUtil.ProfilesCreate();
#endif

	if ( status.IsOk() )
		status = m_importUtil.SystemFlagsSet();

	if ( !status.isError() )
		status += m_importUtil.SmartCAMPlanesDestroy( m_refname );

#if REQUIRED
	// Only required if using a layer map ... which we are not.
	if ( status.IsOk() )
	{
		CAutoMod autoMod;
		autoMod.Init( m_autoModDb, model );
		status = autoMod.DbToolsCreate();
	}
#endif

	CDbWorkplane* dbWork;
	status += m_model->EntityFind( "Top", (CDbEntity**) &dbWork, DBWORKPLANE, DBWORKPLANE );

	if (dbWork != NULL)
		m_model->ActiveWorkplane( dbWork );

	return status;
}


CReturn
CMyPm4::Analyze( const CString& pm4FileName )
{
	CFileException	err;
	CReturn			ret;

	if (m_file.Open( pm4FileName, CFile::modeRead ) )
	{
		m_importUtil.Init( NULL, NULL );
		ret += do_read_layer();

		m_file.Close();
	}
	else
		ret.SystemError();

	return ret;
}

