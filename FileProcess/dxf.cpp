// ============================================================================
//		DXF File Interface
//
// ============================================================================

#include <stdafx.h>
#include "cmn_resource.h"
#include "MathConst.h"
#include "StringConst.h"

#include "3dBox.h"
#include "DbEntity.h"
#include "DbPoint.h"
#include "DbWorkplane.h"
#include "DbIterator.h"
#include "AutoModDb.h"
#include "AutoMod.h"
#include "dxf.h"

static const CString XY_POS = "XY_POS";


// ============================================================================

CDxf::CDxf( void )
	: m_importUtil(),
	  m_model( NULL ),
	  m_autoModDb( NULL ),
	  m_lineNo( 0 )
{
}

CDxf::~CDxf( void )
{
	m_acceptedLayers.DestructiveFlush();
	m_entities.DestructiveFlush();
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

CReturn
CDxf::Read(
		const CString&			dxfpath,
		const CCadLayerList&	acceptedLayers,
		CAutoModDb*				autoModDb )
{
	CReturn	status;

	m_autoModDb = autoModDb;

	AcceptedLayersInit( acceptedLayers );

	status = ReadEntities( dxfpath );

	return status;
}

CReturn
CDxf::ReadEntities( const CString& dxfpath )
{
	CReturn	status;

	m_xorg = 0.;
	m_yorg = 0.;
	m_zorg = 0.;

	m_normal.Init( 0., 0., 1. );

	status = m_file.Open( dxfpath, FILEMODE_READ );
	while ( !m_file.isEOF() && status.isOkay() )
	{
		status += read_pair();

		if ( status.isOkay() )
		{
			switch (m_group)
			{
			case 0:
				if (m_value.CompareNoCase("SECTION") == 0)
				{
					status += read_section();
				}
				break;

			case 999:
				// parse_comment( m_value );
				break;
			}
		}
	}
	m_file.Close();

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void
CDxf::AcceptedLayersInit( const CCadLayerList& acceptedLayers )
{
	CCadLayer*	cadLayer;
	int	count, indx;

	m_acceptedLayers.DestructiveFlush();

	count = acceptedLayers.Count();
	for (indx = 0; indx < count; ++indx)
	{
		cadLayer = acceptedLayers[indx];
		m_acceptedLayers.Append( cadLayer->Name() );
	}

	m_acceptedLayers.Sort();
}

CReturn
CDxf::Analyze( const CString& dxfFileName )
{
	CReturn status = m_file.Open( dxfFileName, FILEMODE_READ );

	if ( status.IsOk() )
		status = read_pair();

	while ( !m_file.isEOF() && status.isOkay() )
	{
		if (m_group == 0 && !m_value.CompareNoCase( "TABLE" ))
		{
			status = read_layer_table();
			break;
		}

		status = read_pair();
	}
	
	m_file.Close();

	return status;
}
