// ==================================================================
// Transform.cpp : Transformation creations
//
// ==================================================================

#include "stdafx.h"

#include "MathConst.h"
#include "CommonFlags.h"
#include "cmn_resource.h"

#include "dbPoint.h"
#include "dbHole.h"
#include "dbLine.h"
#include "dbArc.h"
#include "dbWorkplane.h"

#include "ViewMgr.h"

#include "DbProfile.h"
#include "DbFeature.h"
#include "DbIterator.h"

#include "Profile.h"
#include "Worm.h"
#include "Conversion.h"

#include "ModelUtil.h"

#include "CreateProcess.h"

// ==================================================================

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// ==================================================================
// Transform:Move:
//			sx, sy, sz
//			ex, ey, ez
//			copies
//
CReturn CCreateProcessApp::Move( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn ret;

	// -----------------------------------------------------
	//		Extract command
	//
	double	sx, sy, sz;
	double	ex, ey, ez;
	int		copies;
	
	ret += io_cmd->getReal( "sx", &sx );
	ret += io_cmd->getReal( "sy", &sy );
	ret += io_cmd->getReal( "sz", &sz );

	ret += io_cmd->getReal( "ex", &ex );
	ret += io_cmd->getReal( "ey", &ey );
	ret += io_cmd->getReal( "ez", &ez );

	copies = 0;
	io_cmd->getInt( "copies", &copies );

	if (!ret.isOkay())
	{
		ret.Internal( IDS_TRANSFORM_PARAM_MISSING );
		return ret;
	}

	// -----------------------------------------------------
	//		Transform...
	//
	C3dVec delta = C3dCoord( ex, ey, ez ) - C3dCoord( sx, sy, sz );

	if (EQUAL( delta.Length(), 0.0 ) )
	{
		ret.Internal( IDS_TRANSFORM_EMPTY );
		return ret;
	}

	HCURSOR hCursor = AfxGetApp()->LoadStandardCursor( IDC_WAIT );
	SetCursor( hCursor );

	C3x4Matrix shift;
	shift.setUnit();
	shift.Shift( delta );

	CModel& model = io_cmd->getModel();
	ret += CModelUtil::Transform( &model, shift, copies, (XFORM_NO_SYSTEM | XFORM_NO_ASSOC) );

	hCursor = AfxGetApp()->LoadStandardCursor( IDC_ARROW );
	SetCursor( hCursor );

	return ret;
}

// ==================================================================
// Transform:Rotate:
//			ox, oy
//			ang
//			copies
//
CReturn 
CCreateProcessApp::Rotate( 
	CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn		ret;
	HCURSOR		hCursor;

	// -----------------------------------------------------
	//		Extract command
	//
	double	ox, oy;
	double	angle;
	int		copies;
	
	ret += io_cmd->getReal( "ox", &ox );
	ret += io_cmd->getReal( "oy", &oy );
	ret += io_cmd->getReal( "ang", &angle );
	copies = 0;
	io_cmd->getInt( "copies", &copies );

	if (!ret.isOkay())
	{
		ret.Internal( IDS_TRANSFORM_PARAM_MISSING );
		return ret;
	}
	
	// -----------------------------------------------------
	//		Transform...
	//
	if (EQUAL( angle, 0.0 ) )
	{
		ret.Internal( IDS_TRANSFORM_EMPTY );
		return ret;
	}

	C3x4Matrix	to_origin;
	C3x4Matrix	rotate;
	C3x4Matrix	from_origin;

	to_origin.setUnit();

	to_origin.Shift( C3dVec( -ox, -oy, 0.0 ) );
	rotate.setXYAngle( angle );
	to_origin.InvertTo( &from_origin );

	to_origin.Transform( &rotate );
	rotate.Transform( &from_origin );

	hCursor = AfxGetApp()->LoadStandardCursor( IDC_WAIT );
	SetCursor( hCursor );

	ret += CModelUtil::Transform( &io_cmd->getModel(), from_origin, copies, 
								(XFORM_NO_SYSTEM | XFORM_NO_ASSOC) );

	hCursor = AfxGetApp()->LoadStandardCursor( IDC_ARROW );
	SetCursor( hCursor );

	return ret;
}

// ==================================================================
// Transform:Scale:
//			ox, oy
//			fx, fy
//
CReturn 
CCreateProcessApp::Scale( 
	CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	C3x4Matrix	shift;
	CModel&		model = io_cmd->getModel();
	CReturn		ret;
	HCURSOR		hCursor;

	// -----------------------------------------------------
	//		Extract command
	//
	double	ox, oy;
	double	fx, fy;
	
	ret += io_cmd->getReal( "ox", &ox );
	ret += io_cmd->getReal( "oy", &oy );
	ret += io_cmd->getReal( "fx", &fx );
	ret += io_cmd->getReal( "fy", &fy );

	if (!ret.isOkay())
	{
		ret.Internal( IDS_TRANSFORM_PARAM_MISSING );
		return ret;
	}

	// -----------------------------------------------------
	//		Transform...
	//
	if (EQUAL( fx, 0.0 )
		|| EQUAL( fy, 0.0 )
		|| ( EQUAL( fy, 1.0 )
				&& EQUAL( fy, 1.0 ) 
			)
		)
	{
		ret.Internal( IDS_TRANSFORM_EMPTY );
		return ret;
	}

	C3x4Matrix	to_origin;
	C3x4Matrix	scale;
	C3x4Matrix	from_origin;

	to_origin.setUnit();
	scale.setUnit();

	to_origin.Shift( C3dVec( -ox, -oy, 0.0 ) );
	scale.Scale( fx, fy, 1.0 );
	to_origin.InvertTo( &from_origin );

	to_origin.Transform( &scale );
	scale.Transform( &from_origin );

	hCursor = AfxGetApp()->LoadStandardCursor( IDC_WAIT );
	SetCursor( hCursor );

	ret += CModelUtil::Transform( &io_cmd->getModel(), from_origin, 0, 
								(XFORM_NO_SYSTEM | XFORM_NO_ASSOC) );

	hCursor = AfxGetApp()->LoadStandardCursor( IDC_ARROW );
	SetCursor( hCursor );

	return ret;
}

// ==================================================================
// Transform:Mirror:
//
CReturn 
CCreateProcessApp::Mirror( 
	CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	C3x4Matrix	shift;
	CModel&		model = io_cmd->getModel();
	CReturn		ret;
	HCURSOR		hCursor;

	// -----------------------------------------------------
	//		Extract command
	//
	double	ox, oy;
	BOOL	mx, my;
	int		copies;
	
	ret += io_cmd->getReal( "ox", &ox );
	ret += io_cmd->getReal( "oy", &oy );
	ret += io_cmd->getInt( "mx", &mx );
	ret += io_cmd->getInt( "my", &my );
	copies = 0;
	io_cmd->getInt( "copies", &copies );
	if (copies > 1)
		copies = 1;

	if (!ret.isOkay())
	{
		ret.Internal( IDS_TRANSFORM_PARAM_MISSING );
		return ret;
	}

	if ( !mx && !my )
	{
		ret.Internal( IDS_TRANSFORM_EMPTY );
		return ret;
	}

	// -----------------------------------------------------
	//		Transform...
	//
	C3x4Matrix	to_origin;
	C3x4Matrix	mirror;
	C3x4Matrix	from_origin;

	to_origin.setUnit();
	mirror.setUnit();

	to_origin.Shift( C3dVec( -ox, -oy, 0.0 ) );
	if (mx)
		mirror.Scale( 1.0, -1.0, 1.0 );
	else if (my)
		mirror.Scale( -1.0, 1.0, 1.0 );
	to_origin.InvertTo( &from_origin );

	to_origin.Transform( &mirror );
	mirror.Transform( &from_origin );

	hCursor = AfxGetApp()->LoadStandardCursor( IDC_WAIT );
	SetCursor( hCursor );

	ret += CModelUtil::Transform(
		&io_cmd->getModel(), from_origin, copies, (XFORM_NO_SYSTEM | XFORM_NO_ASSOC) );

	hCursor = AfxGetApp()->LoadStandardCursor( IDC_ARROW );
	SetCursor( hCursor );

	return ret;
}


// ==================================================================
// Transform:MirrorLine:sx=%g,sy=%g,ex=%g,ey=%g,copies=%d
//
//	Mirror in XY plane around 2D line.
//
CReturn 
CCreateProcessApp::MirrorLine( 
	CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	C3x4Matrix	shift;
	CModel&		model = io_cmd->getModel();
	CReturn		ret;
	HCURSOR		hCursor;

	// -----------------------------------------------------
	//		Extract command
	//
	double	sx,sy;
	double	ex,ey;
	int		copies;
	
	ret += io_cmd->getReal( "sx", &sx );
	ret += io_cmd->getReal( "sy", &sy );
	ret += io_cmd->getReal( "ex", &ex );
	ret += io_cmd->getReal( "ey", &ey );
	copies = 0;
	io_cmd->getInt( "copies", &copies );
	if (copies > 1)
		copies = 1;

	if (!ret.isOkay())
	{
		ret.Internal( IDS_TRANSFORM_PARAM_MISSING );
		return ret;
	}

	// -----------------------------------------------------
	//		Transform...
	//
	C2dUnitVec	mline( ex-sx, ey-sy );

	C3x4Matrix	to_origin;
	C3x4Matrix	to_rotate;
	C3x4Matrix	mirror;
	C3x4Matrix	from_rotate;
	C3x4Matrix	from_origin;

	to_origin.setUnit();
	mirror.setUnit();

	to_origin.Shift( C3dVec( -sx, -sy, 0.0 ) );
	to_rotate.setXYAngle( -mline.Radians() );
	mirror.Scale( 1.0, -1.0, 1.0 );
	to_rotate.InvertTo( &from_rotate );
	to_origin.InvertTo( &from_origin );

	to_origin.Transform( &to_rotate );
	to_rotate.Transform( &mirror );
	mirror.Transform( &from_rotate );
	from_rotate.Transform( &from_origin);

	hCursor = AfxGetApp()->LoadStandardCursor( IDC_WAIT );
	SetCursor( hCursor );

	ret += CModelUtil::Transform(
		&io_cmd->getModel(), from_origin, copies, (XFORM_NO_SYSTEM | XFORM_NO_ASSOC) );

	hCursor = AfxGetApp()->LoadStandardCursor( IDC_ARROW );
	SetCursor( hCursor );

	return ret;
}


// ==================================================================
// Transform:Grid: dx=%f, dy=%f, xcnt=%d, ycnt=%d
//
CReturn 
CCreateProcessApp::Grid( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn	status;
	HCURSOR	hCursor;
	double	dx, dy;
	int		xcnt, ycnt;
	
	dx = io_cmd->VarList().getReal( "dx", 0. );
	dy = io_cmd->VarList().getReal( "dy", 0. );
	xcnt = io_cmd->VarList().getInt( "xcnt", 0 );
	ycnt = io_cmd->VarList().getInt( "ycnt", 0 );

	if ((ZERO(dx) && ZERO(dy)) || ((xcnt <= 0) && (ycnt <= 0)))
	{
		status.Internal( IDS_INTERNAL_ERROR, "CCreateProcessApp::Grid(#1)" );
	}
	else
	{
		hCursor = AfxGetApp()->LoadStandardCursor( IDC_WAIT );
		SetCursor( hCursor );

		status = CModelUtil::TransformGrid( &io_cmd->getModel(), dx, dy, xcnt, ycnt );

		hCursor = AfxGetApp()->LoadStandardCursor( IDC_ARROW );
		SetCursor( hCursor );
	}

	return status;
}
